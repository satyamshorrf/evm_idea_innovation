from pyfingerprint.pyfingerprint import PyFingerprint

try:
    f = PyFingerprint('COM31', 57600, 0xFFFFFFFF, 0x00000000)

    if not f.verifyPassword():
        raise ValueError("The given fingerprint sensor password is wrong!")

    print("Connected to sensor ✅")
    print("Currently stored templates: " + str(f.getTemplateCount()) + "/" + str(f.getStorageCapacity()))

except Exception as e:
    print("Could not initialize fingerprint sensor!")
    print("Exception message: " + str(e))
