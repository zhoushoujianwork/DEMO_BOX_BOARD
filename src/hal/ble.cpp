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
static BLERemoteCharacteristic *pRemoteCharacteristic;
static BLERemoteCharacteristic *pRemoteIMUCharacteristic;
static BLERemoteCharacteristic *pRemoteGPSCharacteristic;
static BLEAdvertisedDevice *myDevice;

static void notifyCallback(
    BLERemoteCharacteristic *pRemoteCharacteristic,
    uint8_t *pData,
    size_t length,
    bool isNotify)
{
    Serial.print("Notify callback for characteristic ");
    Serial.print(pRemoteCharacteristic->getUUID().toString().c_str());
    Serial.print(" of data length ");
    Serial.println(length);
    if (length == sizeof(device_state_t))
    {
        memcpy(get_remote_device_state(), pData, sizeof(device_state_t));
    }
}

class MyClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient)
    {
        Serial.println("onConnect");
        connected = true;
        get_local_device_state()->bleConnected = true;
    }

    void onDisconnect(BLEClient *pclient)
    {
        connected = false;
        get_local_device_state()->bleConnected = false;
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
    pRemoteCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID);
    if (pRemoteCharacteristic == nullptr)
    {
        Serial.print("Failed to find our CHARACTERISTIC_UUID characteristic UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    Serial.println(" - Found our characteristic");
    if (pRemoteCharacteristic->canNotify())
        pRemoteCharacteristic->registerForNotify(notifyCallback);

    pRemoteIMUCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_IMU);
    if (pRemoteIMUCharacteristic == nullptr)
    {
        Serial.print("Failed to find our CHARACTERISTIC_UUID_IMU characteristic UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }
    pRemoteGPSCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_UUID_GPS);
    if (pRemoteGPSCharacteristic == nullptr)
    {
        Serial.print("Failed to find our CHARACTERISTIC_UUID_GPS characteristic UUID: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
        return false;
    }

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
    // pBLEScan->start(0, false);
}

// This is the Arduino main loop function.
void ble_read_from_server()
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
        get_remote_device_state()->bleConnected = true;
        // read
        if (pRemoteIMUCharacteristic->canRead())
        {
            std::string value = pRemoteIMUCharacteristic->readValue();
            if (value.length() == sizeof(imu_data_t))
            {
                get_imu_data()->gx = ((imu_data_t *)value.data())->gx;
                get_imu_data()->gy = ((imu_data_t *)value.data())->gy;
                get_imu_data()->gz = ((imu_data_t *)value.data())->gz;
                get_imu_data()->ax = ((imu_data_t *)value.data())->ax;
                get_imu_data()->ay = ((imu_data_t *)value.data())->ay;
                get_imu_data()->az = ((imu_data_t *)value.data())->az;
                get_imu_data()->roll = ((imu_data_t *)value.data())->roll;
                get_imu_data()->pitch = ((imu_data_t *)value.data())->pitch;
                get_imu_data()->yaw = ((imu_data_t *)value.data())->yaw;
                get_imu_data()->temperature = ((imu_data_t *)value.data())->temperature;
            }
            else
            {
                Serial.println("Received imu data length mismatch");
            }
        }

        if (pRemoteGPSCharacteristic->canRead())
        {
            std::string value = pRemoteGPSCharacteristic->readValue();
            if (value.length() == sizeof(gps_data_t))
            {
                get_gps_data()->satellites = ((gps_data_t *)value.data())->satellites;
                get_gps_data()->lat = ((gps_data_t *)value.data())->lat;
                get_gps_data()->lng = ((gps_data_t *)value.data())->lng;
                get_gps_data()->altitude = ((gps_data_t *)value.data())->altitude;
                get_gps_data()->speed = ((gps_data_t *)value.data())->speed;
                get_gps_data()->direction = ((gps_data_t *)value.data())->direction;
                get_gps_data()->year = ((gps_data_t *)value.data())->year;
                get_gps_data()->month = ((gps_data_t *)value.data())->month;
                get_gps_data()->day = ((gps_data_t *)value.data())->day;
                get_gps_data()->hour = ((gps_data_t *)value.data())->hour;
                get_gps_data()->minute = ((gps_data_t *)value.data())->minute;
                get_gps_data()->second = ((gps_data_t *)value.data())->second;
                get_gps_data()->centisecond = ((gps_data_t *)value.data())->centisecond;
            }
            else
            {
                Serial.println("Received gps data length mismatch");
            }
        }
    }
    else if (doScan)
    {
        get_remote_device_state()->bleConnected = false;
        Serial.println("scanning");
        BLEDevice::getScan()->start(10, false);
        doScan = false;
    }

    if (doConnect != true && connected != true && doScan != true)
    {
        doScan = true;
    }
}
