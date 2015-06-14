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


// ========================================================================== //
// Get display name.
// ========================================================================== //
QString ADeviceIdentifier::displayName() const {
    const bool has_url = _types.contains(ADeviceIdentifier::TYPE_URL);
    const bool has_dev = _types.contains(ADeviceIdentifier::TYPE_DEV);
    const bool has_grp = _types.contains(ADeviceIdentifier::TYPE_GRP);

    if(has_url) {
        return _types.value(ADeviceIdentifier::TYPE_URL).toString();

    } else {
        if(has_dev && has_grp) {
            const QString dev
                = _types.value(ADeviceIdentifier::TYPE_DEV).toString();

            const QString grp
                = _types.value(ADeviceIdentifier::TYPE_GRP).toString();

            return grp + QLatin1String(" - ") + dev;

        } else if(has_dev) {
            return _types.value(ADeviceIdentifier::TYPE_DEV).toString();

        } else if(has_grp) {
            return _types.value(ADeviceIdentifier::TYPE_GRP).toString();
        }
    }

    return QString();
}
