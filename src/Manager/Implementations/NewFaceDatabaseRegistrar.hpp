/**
 * @file NewFaceDatabaseRegistrar.hpp
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 13 Sep 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_NEWFACEDATABASEREGISTRAR_HPP
#define FACES_NEWFACEDATABASEREGISTRAR_HPP

#include <utils/utils.h>
#include <Manager/Manager.hpp>
#include <Database/Database.hpp>

namespace faces {

    template<typename DatabaseT>
    class NewFaceDatabaseRegistrar : public NewFaceHandlerBase {
        static_assert(is_base_template_of<Database, std::remove_pointer_t<DatabaseT>>::value,
                      "DatabaseT should be an implementation of the Database");
        static_assert(std::is_convertible_v<Face const &, typename DatabaseT::EntryType>,
                      "DatabaseT's EntryT should be initialisable with Face");

    public:
        inline static bool saveDatabase = true;

        explicit NewFaceDatabaseRegistrar(DatabaseT &database) : _database(database) {}

        void handle(Face &face) override {
            auto entry = static_cast<typename DatabaseT::EntryType>(face);
            typename DatabaseT::IdentifierType newId = _database.add(entry);
            if (saveDatabase) {
                _database.save();
            }

            face.label = static_cast<int>(newId);
        }

    private:
        DatabaseT _database;

    };

}

#endif //FACES_NEWFACEDATABASEREGISTRAR_HPP
