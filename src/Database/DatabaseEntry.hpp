/**
 * @file DatabaseEntry.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 25 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DATABASEENTRY_HPP
#define FACES_DATABASEENTRY_HPP

#include <utils/LookableAttributes.hpp>

namespace faces {

    template<typename DerivedT>
    class DatabaseEntry : public LookableFields<DerivedT> {
    public:
        bool initAttributes(std::map<std::string, std::any> const &attributes) {
            bool ok = true;
            for (auto const &[name, value] : attributes) {
                try {
                    LookableFields<DerivedT>::set(name, value);
                } catch (std::exception const &e) {
                    spdlog::warn("Error during initialization of database entry`s fields: {}", e.what());
                    ok = false;
                }
            }
            return ok;
        }

        [[nodiscard]] std::map<std::string, std::any> getAttributes() const {
            std::map<std::string, std::any> res;

            for (std::string const &attribute : this->_attributeNames) {
                res[attribute] = LookableFields<DerivedT>::get(attribute);
            }

            return res;
        }

        virtual bool load() = 0;

    protected:
        explicit DatabaseEntry(DerivedT &derived) : LookableFields<DerivedT>(derived) {}

        using LookableFields<DerivedT>::_registerField;

    };

}

#endif //FACES_DATABASEENTRY_HPP
