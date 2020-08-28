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

#include <sstream>
#include <any>

#include <semimap.h>

#define FACES_REGISTER_ACCESSOR(cls, attribute) \
    int __faces_attribute_accessor_dymmy_ ## attribute = _registerField(#attribute, &cls::attribute);

namespace faces {

    template<typename T>
    std::string getTypeStr() {
        std::string typeStr = __PRETTY_FUNCTION__;
        std::size_t start = typeStr.find("[with T = ") + 10;
        std::size_t end = typeStr.find(';');

        return typeStr.substr(start, end - start);
    }

#define _FACES_TRY_CAST_ANY(dstType, triedType) \
            if(value.type() == typeid(triedType)) { \
                auto realValue = std::any_cast<triedType>(value); \
                return static_cast<dstType>(realValue); \
            }

#define _FACES_ADD_ANY_CAST_FOR_NUM(forType) \
            template <> \
            forType anyCast(std::any const &value) { \
                _FACES_TRY_CAST_ANY(forType, int) \
                _FACES_TRY_CAST_ANY(forType, unsigned int) \
                _FACES_TRY_CAST_ANY(forType, float) \
                _FACES_TRY_CAST_ANY(forType, double) \
                _FACES_TRY_CAST_ANY(forType, short) \
                _FACES_TRY_CAST_ANY(forType, unsigned short) \
                _FACES_TRY_CAST_ANY(forType, std::size_t) \
                return std::any_cast<forType>(value); \
            }

    template<typename T>
    T anyCast(std::any const &value) {
        return std::any_cast<T>(value);
    }

    _FACES_ADD_ANY_CAST_FOR_NUM(int)
    _FACES_ADD_ANY_CAST_FOR_NUM(unsigned int)
    _FACES_ADD_ANY_CAST_FOR_NUM(double)
    _FACES_ADD_ANY_CAST_FOR_NUM(float)
    _FACES_ADD_ANY_CAST_FOR_NUM(short)
    _FACES_ADD_ANY_CAST_FOR_NUM(unsigned short)
    _FACES_ADD_ANY_CAST_FOR_NUM(std::size_t)

#undef _FACES_TRY_CAST_ANY
#undef _FACES_ADD_ANY_CAST_FOR_NUM

    template<typename BaseClass>
    class AttributeAccessor {
    public:
        const std::string key;

        explicit AttributeAccessor(std::string aKey) : key(std::move(aKey)) {}

        virtual void set(BaseClass &p, std::any const &value) const = 0;

        virtual std::any get(BaseClass &base) const = 0;
    };

    template<typename BaseClass, typename T>
    class AttributeAccessorImpl : public AttributeAccessor<BaseClass> {
    public:
        typedef T BaseClass::*TMember;
        TMember member;

        AttributeAccessorImpl(const std::string &aKey, const TMember t)
                : AttributeAccessor<BaseClass>(aKey), member(t) {}

        void set(BaseClass &p, std::any const &value) const override {
            (p.*member) = anyCast<T>(value);
        }

        std::any get(BaseClass &base) const override {
            return base.*member;
        }
    };

    template<typename DerivedT>
    class LookableFields {
    public:
        void set(std::string const &name, std::any const &value) {
            if (!_setters::contains(name)) {
                std::stringstream ss;
                ss << "Attribute '" << name << "' of the class '" << getTypeStr<DerivedT>() << "' not found";
                throw std::out_of_range(ss.str());
            }

            _setters::get(name)->set(_derived, value);
        }

        [[nodiscard]] std::any get(std::string const &name) const {
            if (!_setters::contains(name)) {
                return std::any();
            }
            return _setters::get(name)->get(_derived);
        }

    protected:
        struct __Tag {
        };
        using _setters = semi::static_map<std::string, AttributeAccessor<DerivedT> *, __Tag>;
        static std::vector<std::string> _attributeNames;

        explicit LookableFields(DerivedT &derived) : _derived(derived) {}

        template<typename FieldT>
        int _registerField(std::string const &name, FieldT DerivedT::*field) {
            _setters::get(name) = new AttributeAccessorImpl<DerivedT, FieldT>(name, field);
            _attributeNames.emplace_back(name);
            return 0;
        }

    private:
        DerivedT &_derived;

    };

    template<typename DerivedT>
    std::vector<std::string> LookableFields<DerivedT>::_attributeNames = {};

}

#endif //FACES_LOOKABLEATTRIBUTES_HPP
