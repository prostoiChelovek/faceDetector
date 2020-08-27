/**
 * @file Database.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 26 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_DATABASE_HPP
#define FACES_DATABASE_HPP

#include "DatabaseEntry.hpp"

namespace faces {

    template<typename EntryT, typename IdentifierT>
    class Database {
    public:
        static_assert(std::is_constructible<EntryT, std::map<std::string, std::any>>::value,
                      "EntryT of the Database should be a subclass of faces::DatabaseEntry, "
                      "and provide access to its protected constructor");

        virtual EntryT get(IdentifierT id) = 0;

        virtual std::vector<IdentifierT> getEntriesList() = 0;

        virtual IdentifierT add(EntryT const &entry) = 0;

        virtual void update(IdentifierT id, EntryT const &entry) = 0;

        virtual bool save() = 0;

        virtual bool load() = 0;

    };

}

#endif //FACES_DATABASE_HPP
