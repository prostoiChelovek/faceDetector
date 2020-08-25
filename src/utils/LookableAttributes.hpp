/**
 * @file LookableAttributes.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 25 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_LOOKABLEATTRIBUTES_HPP
#define FACES_LOOKABLEATTRIBUTES_HPP

#include <string>
#include <utility>

#include <semimap.h>
#include <any>

#define FACES_REGISTER_ACCESSOR(cls, attribute) \
    int __faces_attribute_accessor_dymmy_ ## attribute = _registerField(#attribute, &cls::attribute);

namespace faces {

    template<typename BaseClass>
    class AttributeAccessor {
    public:
        const std::string key;

        explicit AttributeAccessor(std::string aKey) : key(std::move(aKey)) {}

        virtual void set(BaseClass &p, std::any const &value) const = 0;

        virtual std::string get(BaseClass &base) const = 0;
    };

    template<typename BaseClass, typename T>
    class AttributeAccessorImpl : public AttributeAccessor<BaseClass> {
    public:
        typedef T BaseClass::*TMember;
        TMember member;

        AttributeAccessorImpl(const std::string &aKey, const TMember t)
                : AttributeAccessor<BaseClass>(aKey), member(t) {}

        void set(BaseClass &p, std::any const &value) const override {
            (p.*member) = std::any_cast<T>(value);
        }

        std::string get(BaseClass &base) const {
            T value = base.*member;
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
    };

    template<typename DerivedT>
    class LookableFields {
    public:
        void set(std::string const &name, std::any const &value) {
            _setters::get(name)->set(_derived, value);
        }

        std::string get(std::string const &name) {
            return _setters::get(name)->get(_derived);
        }

    protected:
        struct __Tag {
        };
        using _setters = semi::static_map<std::string, AttributeAccessor<DerivedT> *, __Tag>;

        explicit LookableFields(DerivedT &derived) : _derived(derived) {}

        template<typename FieldT>
        int _registerField(std::string const &name, FieldT DerivedT::*field) {
            _setters::get(name) = new AttributeAccessorImpl<DerivedT, FieldT>(name, field);
            return 0;
        }

    private:
        DerivedT &_derived;

    };

}

#endif //FACES_LOOKABLEATTRIBUTES_HPP
