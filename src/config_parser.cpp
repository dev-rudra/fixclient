#include "config_parser.h"

#include <fstream>
#include <stdexcept>
#include <cstdlib>

// Removes
// spaces, tabs, newlines
// from the start and end of string
std::string ConfigParser::trim(const std::string& str) {
    size_t start_pos = str.find_first_not_of(" \t\r\n");
    if (start_pos == std::string::npos) {
        return "";
    }

    size_t end_pos = str.find_last_not_of(" \t\r\n");
    return str.substr(start_pos, end_pos - start_pos + 1);
}

void ConfigParser::load(const std::string& path) {
    std::ifstream file(path.c_str());
    if (!file.is_open()) {
        throw std::runtime_error("Error: Cannot open file: " + path);
    }

    sessions.clear();
    
    std::string section_name = "DEFAULT";
    std::string line;

    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        if (line[0] == '#' || line[0] == ';') {
            continue;
        }

        if (line.front() == '[' && line.back() == ']') {
            section_name = trim(line.substr(1, line.size() -2));
            if (section_name.empty()) {
                section_name = "DEFAULT";
            }
            continue;
        }

        size_t equal_pos = line.find('=');
        if (equal_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, equal_pos));
        std::string value = trim(line.substr(equal_pos +1));
        if (key.empty()) {
            continue;
        }

        SessionConfig* config = 0;
        if (section_name == "DEFAULT") {
            config = &defaults;
        } else {
            SessionConfig& session_config = sessions[section_name];
            if (session_config.name.empty()) {
                session_config = defaults;
                session_config.name = section_name;
            }
            config = &session_config;
        }

        if (key == "host") config->host = value;
        else if (key == "port") config->port = std::atoi(value.c_str());
        else if (key == "begin_string") config->begin_string = value;
        else if (key == "sender_comp_id") config->sender_comp_id = value;
        else if (key == "target_comp_id") config->target_comp_id = value;
        else if (key == "heartbeat_interval") config->heartbeat_interval = std::atoi(value.c_str());
        else if (key == "reset_on_logon") config->reset_on_logon = (value == "true");
    }
}

SessionConfig ConfigParser::get_session(const std::string& session_name) const {
    std::map<std::string, SessionConfig>::const_iterator found = sessions.find(session_name);
    if (found == sessions.end()) {
        throw std::runtime_error("Error: Session not found: " + session_name);
    }
    return found->second;
}
