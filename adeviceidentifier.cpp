#include "adeviceidentifier.h"

// ========================================================================== //
// Constructor.
// ========================================================================== //
ADeviceIdentifier::ADeviceIdentifier() {}


// ========================================================================== //
// Get has value.
// ========================================================================== //
bool ADeviceIdentifier::hasValue(ADeviceIdentifier::Type type) const {
    return _types.contains(type);
}


// ========================================================================== //
// Get type value.
// ========================================================================== //
QVariant ADeviceIdentifier::value(ADeviceIdentifier::Type type) const {
    return _types.value(type);
}


// ========================================================================== //
// Set type value.
// ========================================================================== //
void ADeviceIdentifier::setValue(ADeviceIdentifier::Type type
    , const QVariant &value) {

    if(value.isValid()) _types.insert(type, value);
    else if(_types.contains(type)) _types.remove(type);
}
