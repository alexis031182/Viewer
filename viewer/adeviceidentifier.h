#ifndef ADEVICEIDENTIFIER_H
#define ADEVICEIDENTIFIER_H

#include <QtCore/QVariant>
#include <QtCore/QHash>

class ADeviceIdentifier {
    public:
        enum Type {TYPE_URL, TYPE_DEV, TYPE_GRP};

        //! Constructor.
        explicit ADeviceIdentifier();

        //! Destructor.
        virtual ~ADeviceIdentifier() {}

        //! Get has value.
        bool hasValue(ADeviceIdentifier::Type type) const;

        //! Get type value.
        QVariant value(ADeviceIdentifier::Type type) const;

        //! Set type value.
        void setValue(ADeviceIdentifier::Type type, const QVariant &value);

        //! Get display name.
        QString displayName() const;

    private:
        QHash<Type,QVariant> _types;

};

#endif
