#include "ble.h"
#include "em_config.h"
#include "em_device.h"
// 本端是作为蓝牙 client端开发，订阅已有的蓝牙服务 并通过读取 GPS 和 IMU 数据
// The remote service we wish to connect to.
static BLEUUID serviceUUID(SERVICE_UUID);
// The characteristic of the remote service we are interested in.
static BLEUUID charServerUUID(CHARACTERISTIC_UUID);
static BLEUUID charGPSUUID(CHARACTERISTIC_UUID_GPS);
static BLEUUID charIMUUUID(CHARACTERISTIC_UUID_IMU);

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic *pRemoteIMUCharacteristic;
static BLERemoteCharacteristic *pRemoteGPSCharacteristic;
static BLEAdvertisedDevice *myDevice;

static void notifyCallback(
    BLERemoteCharacteristic *pBLERemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    Serial.print("Notify callback for characteristic ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    Serial.print("data: ");
    Serial.println((char *)pData);
    if (length == sizeof(device_state_t))
    {
        device_state_t g_device_state;
        memcpy(&g_device_state, pData, sizeof(device_state_t));
        Serial.printf("gps_state = %d, imu_state = %d, battery = %d, temperature = %d\n", g_device_state.gps_state, g_device_state.imu_state, g_device_state.battery, g_device_state.temperature);
    }
}

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
        Serial.println("onConnect");
        connected = true;
    }

    void onDisconnect(BLEClient *pclient)
    {
        connected = false;
        Serial.println("onDisconnect");
        doScan = true;
    }
};

bool connectToServer()
{
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());

    BLEClient *pClient = BLEDevice::createClient();

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(myDevice); // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr)
    {
        Serial.print("Failed to find our service UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteIMUCharacteristic = pRemoteService->getCharacteristic(charIMUUUID);
    if (pRemoteIMUCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");
    if (pRemoteIMUCharacteristic->canNotify())
        pRemoteIMUCharacteristic->registerForNotify(notifyCallback);

    pRemoteGPSCharacteristic = pRemoteService->getCharacteristic(charGPSUUID);
    if (pRemoteGPSCharacteristic == nullptr)
    {
        Serial.print("Failed to find our characteristic UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");
    if (pRemoteGPSCharacteristic->canNotify())
        pRemoteGPSCharacteristic->registerForNotify(notifyCallback);

    return true;
}
/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    /**
     * Called for each advertising BLE server.
     */
    void onResult(BLEAdvertisedDevice advertisedDevice)
    {
        Serial.print("BLE Advertised Device found: ");
        Serial.println(advertisedDevice.toString().c_str());

        // We have found a device, let us now see if it contains the service we are looking for.
        if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID))
        {

            BLEDevice::getScan()->stop();
            myDevice = new BLEAdvertisedDevice(advertisedDevice);
            doConnect = true;
        } // Found our server
    } // onResult
}; // MyAdvertisedDeviceCallbacks

void setup_ble()
{
    Serial.println("Starting Arduino BLE Client application...");
    BLEDevice::init("");

    BLEScan *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(0, false);
}

// This is the Arduino main loop function.
void loop_ble()
{
    // Serial.printf("\ndoConnect = %d, connected = %d, doScan = %d\n", doConnect, connected, doScan);
    // If the flag "doConnect" is true then we have scanned for and found the desired
    // BLE Server with which we wish to connect.  Now we connect to it.  Once we are
    // connected we set the connected flag to be true.
    if (doConnect == true)
    {
        Serial.println("Connecting....");
        if (connectToServer())
        {
            Serial.println("We are now connected to the BLE Server.");
            doConnect = false;
        }
        else
        {
            Serial.println("We have failed to connect to the server; there is nothin more we will do.");
        }
    }

    // If we are connected to a peer BLE Server, update the characteristic each time we are reached
    // with the current time since boot.
    if (connected)
    {
        // read
        if (pRemoteIMUCharacteristic->canRead())
        {
            std::string value = pRemoteIMUCharacteristic->readValue();
            if (value.length() == sizeof(imu_data_t))
            {
                imu_data_t imu;
                memcpy(&imu, value.data(), sizeof(imu_data_t));

                Serial.print("\naccX: ");
                Serial.print(imu.ax);
                Serial.print("\taccY: ");
                Serial.print(imu.ay);
                Serial.print("\taccZ: ");
                Serial.print(imu.az);
                Serial.print("\tgyroX: ");
                Serial.print(imu.gx);
                Serial.print("\tgyroY: ");
                Serial.print(imu.gy);
                Serial.print("\tgyroZ: ");
                Serial.print(imu.gz);
                Serial.print("\ttemp: ");
                Serial.print(imu.temperature);
            }
            else
            {
                Serial.println("Received data length mismatch");
            }
        }

        if (pRemoteGPSCharacteristic->canRead())
        {
            std::string value = pRemoteGPSCharacteristic->readValue();
            if (value.length() == sizeof(gps_data_t))
            {
                gps_data_t gps;
                memcpy(&gps, value.data(), sizeof(gps_data_t));

                Serial.print("\nyear: ");
                Serial.print(gps.year);
                Serial.print("\tmonth: ");
                Serial.print(gps.month);
                Serial.print("\tday: ");
                Serial.print(gps.day);
                Serial.print("\thour: ");
                Serial.print(gps.hour);
                Serial.print("\tminute: ");
                Serial.print(gps.minute);
                Serial.print("\tsecond: ");
                Serial.print(gps.second);
                Serial.print("\tcentisecond: ");
                Serial.print(gps.centisecond);
                Serial.print("\tlat: ");
                Serial.print(gps.lat);
                Serial.print("\tlng: ");
                Serial.print(gps.lng);
                Serial.print("\taltitude: ");
                Serial.print(gps.altitude);
                Serial.print("\tspeed: ");
                Serial.print(gps.speed);
                Serial.print("\tdirection: ");
                Serial.print(gps.direction);
                Serial.print("\tsatellites: ");
                Serial.print(gps.satellites);
            }
            else
            {
                Serial.println("Received data length mismatch");
            }
        }
    }
    else if (doScan)
    {
        Serial.println("scanning");
        BLEDevice::getScan()->start(0); // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
    }

} // End of loop