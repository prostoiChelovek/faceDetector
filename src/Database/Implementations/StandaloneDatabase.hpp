/**
 * @file StandaloneDatabase.h
 * @author Люнгрин Андрей aka prostoichelovek <iam.prostoi.chelovek@gmail.com>
 * @date 26 Aug 2020
 * @copyright MIT License
 *
 * @brief
 */

#ifndef FACES_STANDALONEDATABASE_HPP
#define FACES_STANDALONEDATABASE_HPP

#include <utility>
#include <fstream>

#include <picojson.h> // from third-party/miniconf

#include <Database/Database.hpp>

namespace faces {

    template<typename EntryT, typename IdentifierT = unsigned long>
    class StandaloneDatabase : public Database<EntryT, IdentifierT> {
    public:
        explicit StandaloneDatabase(std::string path) : _path(std::move(path)) {}

        EntryT get(IdentifierT id) override {
            picojson::object const &entryObj = _getEntryJson(id);

            std::map<std::string, std::any> entryAttributes;
            for (auto const &[name, valueJson] : entryObj) {
                std::any value = _valueToAny(valueJson);
                entryAttributes[name] = value;
            }

            return EntryT(entryAttributes);
        }

        std::vector<IdentifierT> getEntriesList() override {
            std::vector<IdentifierT> res;

            auto &obj = _data.get<picojson::object>();
            for (auto const &[key, value] : obj) {
                if constexpr (!std::is_same_v<picojson::object::key_type, IdentifierT>) {
                    if constexpr (std::is_arithmetic_v<IdentifierT>) {
                        std::istringstream iss(key);
                        IdentifierT id;
                        iss >> id;
                        res.emplace_back(id);
                    } else {
                        res.emplace_back(static_cast<IdentifierT>(key));
                    }
                } else {
                    res.emplace_back(key);
                }
            }

            return res;
        }

        IdentifierT add(EntryT const &entry) override {}

        void update(IdentifierT id, EntryT const &entry) override {
            picojson::object &entryObj = _getEntryJson(id);

            std::map<std::string, std::any> attributes = entry.getAttributes();
            for (auto const &[key, value] : attributes) {
                entryObj[key] = _anyToValue(value);
            }
        }

        bool save() override {
            try {
                std::ofstream file(_path);
                file.exceptions(file.exceptions() | std::ifstream::failbit | std::ifstream::badbit);

                if (!_data.is<picojson::object>()) {
                    _data = picojson::value(picojson::object());
                }

                _data.serialize(std::ostream_iterator<char>(file), true);
            } catch (std::exception &e) {
                spdlog::error("Cannot save a database to '{}': {}", _path, e.what());
                return false;
            }

            return true;
        }

        bool load() override {
            try {
                std::ifstream file(_path);
                if (!file.is_open() || file.bad()) {
                    throw std::ios_base::failure(strerror(errno));
                }

                _loadJson(file);
            } catch (std::exception &e) {
                spdlog::error("Cannot load a database from '{}': {}", _path, e.what());
                return false;
            }

            return true;
        }

    protected:
        const std::string _path;

        picojson::value _data;

        void _loadJson(std::ifstream &file) {
            std::string err = picojson::parse(_data, file);
            if (!err.empty()) {
                throw std::logic_error(err);
            }
            if (!_data.is<picojson::object>()) {
                throw std::logic_error("json file has an incorrect format");
            }
        }

        static std::any _valueToAny(picojson::value const &value) {
            if (value.is<bool>()) {
                return value.get<bool>();
            }
            if (value.is<double>()) {
                return value.get<double>();
            }
            if (value.is<std::string>()) {
                return value.get<std::string>();
            }
            if (value.is<picojson::object>()) {
                return value.get<picojson::object>();
            }
            throw std::bad_cast();
        }

        static picojson::value _anyToValue(std::any const &value) {
            if (value.type() == typeid(bool)) {
                return picojson::value(std::any_cast<bool>(value));
            }
            if (value.type() == typeid(std::string)) {
                return picojson::value(std::any_cast<std::string>(value));
            }
            return picojson::value(anyCast<double>(value));
        }

        picojson::object &_getEntryJson(IdentifierT const &id) {
            picojson::object &obj = _data.get<picojson::object>();

            std::stringstream ss;
            ss << id;
            std::string strId = ss.str();

            if (obj.find(strId) == obj.end()) {
                throw std::out_of_range("Entry with ID='" + strId + "' does not exist in the database");
            }

            picojson::value &entryJson = obj[strId];
            if (!entryJson.is<picojson::object>()) {
                throw std::logic_error("Entry with ID='" + strId + "' has an incorrect format");
            }
            return entryJson.get<picojson::object>();
        }

    };

}

#endif //FACES_STANDALONEDATABASE_HPP
