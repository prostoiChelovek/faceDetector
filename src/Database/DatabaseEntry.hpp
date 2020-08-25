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

    class DatabaseEntry : public LookableFields<DatabaseEntry> {
    public:
        DatabaseEntry() : LookableFields<DatabaseEntry>(*this) {}

        int test;
        FACES_REGISTER_ACCESSOR(DatabaseEntry, test)

        std::string str;
        FACES_REGISTER_ACCESSOR(DatabaseEntry, str)

    };

}

#endif //FACES_DATABASEENTRY_HPP
