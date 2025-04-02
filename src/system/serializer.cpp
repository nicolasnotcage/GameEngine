/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#include "system/serializer.hpp"
#include "system/string_utils.hpp"

#include <fstream>
#include <string>
#include <cstring>
#include <iostream>

namespace cge
{
//--------------------------
//     Text Serializer
//--------------------------
bool TextSerializer::open(const std::string& filepath, bool write_mode)
{
    filepath_ = filepath;
    is_write_mode_ = write_mode;

    // Clear existing data
    data_.clear();

    // If not in write mode, load the file
    if (!write_mode)
    {
        auto lines = cge::utility::lines_from_file(filepath);
        for (const auto& line : lines)
        {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;

            // Parse key-value pairs (format: Key = Value)
            auto parts = cge::utility::split(line, "=");
            if (parts.size() >= 2) {
                std::string key = cge::utility::trim(parts[0]);
                std::string value = cge::utility::trim(parts[1]);
                data_[key] = value;
            }
        }
    }

    is_open_ = true;
    return true;
}

void TextSerializer::close()
{
    is_open_ = false;
}

bool TextSerializer::save()
{
    if (!is_write_mode_) return false;

    std::ofstream file(filepath_);
    if (!file.is_open()) return false;

    // Write header
    file << "# Configuration File" << std::endl;
    file << "# Generated on: " << __DATE__ << " " << __TIME__ << std::endl;
    file << std::endl;

    // Write data
    for (const auto& [key, value] : data_)
    {
        file << key << " = " << value << std::endl;
    }

    file.close();
    return true;
}

void TextSerializer::write(const std::string& key, const std::string& value)
{
    data_[key] = value;
}

void TextSerializer::write(const std::string& key, int value)
{
    data_[key] = std::to_string(value);
}

void TextSerializer::write(const std::string& key, float value)
{
    data_[key] = std::to_string(value);
}

void TextSerializer::write(const std::string& key, bool value)
{
    data_[key] = value ? "true" : "false";
}

bool TextSerializer::read(const std::string& key, std::string& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        value = it->second;
        return true;
    }
    return false;
}

bool TextSerializer::read(const std::string& key, int& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        try
        {
            value = std::stoi(it->second);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

bool TextSerializer::read(const std::string& key, float& value)
{
    auto it = data_.find(key);
    if (it != data_.end())
    {
        try
        {
            value = std::stof(it->second);
            return true;
        }
        catch (...)
        {
            return false;
        }
    }
    return false;
}

bool TextSerializer::read(const std::string& key, bool& value) {
    auto it = data_.find(key);
    if (it != data_.end()) {
        std::string lower = cge::utility::to_lower(it->second);
        if (lower == "true" || lower == "1")
        {
            value = true;
            return true;
        }
        else if (lower == "false" || lower == "0")
        {
            value = false;
            return true;
        }
    }
    return false;
}

//--------------------------
//     Binary Serializer
//--------------------------
bool BinarySerializer::open(const std::string& filepath, bool write_mode)
{
    filepath_ = filepath;
    is_write_mode_ = write_mode;

    // Debug output
    std::cout << "BinarySerializer::open - " << (write_mode ? "Writing to" : "Reading from") 
              << " file: " << filepath_ << std::endl;

    // Clear existing data
    data_buffer_.clear();
    data_map_.clear();

    if (!write_mode)
    {
        // Load file contents into buffer
        std::ifstream file(filepath_, std::ios::binary);
        if (!file.is_open()) {
            std::cout << "BinarySerializer::open - Failed to open file for reading" << std::endl;
            return false;
        }

        // Get file size
        file.seekg(0, std::ios::end);
        std::streampos fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::cout << "BinarySerializer::open - File size: " << fileSize << " bytes" << std::endl;

        if (fileSize > 0) {
            // First read the number of entries in the index table
            int num_entries;
            file.read(reinterpret_cast<char*>(&num_entries), sizeof(int));
            
            std::cout << "BinarySerializer::open - Number of entries in index table: " << num_entries << std::endl;
            
            // Read the index table
            size_t index_table_size = 0;
            for (int i = 0; i < num_entries; ++i) {
                // Read key size
                int key_size;
                file.read(reinterpret_cast<char*>(&key_size), sizeof(int));
                
                // Read key
                std::string key(key_size, '\0');
                file.read(&key[0], key_size);
                
                // Read offset and size
                size_t offset, size;
                file.read(reinterpret_cast<char*>(&offset), sizeof(size_t));
                file.read(reinterpret_cast<char*>(&size), sizeof(size_t));
                
                // Store in map
                data_map_[key] = { offset, size };
                
                std::cout << "BinarySerializer::open - Read key: " << key 
                          << ", offset: " << offset << ", size: " << size << std::endl;
                
                // Calculate the size of the index table entry
                index_table_size += sizeof(int) + key_size + sizeof(size_t) * 2;
            }
            
            // Calculate the total size of the index table including the num_entries
            index_table_size += sizeof(int);
            
            // Calculate the size of the data buffer
            size_t data_buffer_size = static_cast<size_t>(fileSize) - index_table_size;
            
            // Read the data buffer
            data_buffer_.resize(data_buffer_size);
            file.read(data_buffer_.data(), data_buffer_size);
            
            std::cout << "BinarySerializer::open - Read data buffer of size: " << data_buffer_size << " bytes" << std::endl;
        } else {
            std::cout << "BinarySerializer::open - Empty file" << std::endl;
        }
        file.close();
    }

    is_open_ = true;
    std::cout << "BinarySerializer::open - Successfully opened file" << std::endl;
    return true;
}

void BinarySerializer::close()
{
    is_open_ = false;
}

bool BinarySerializer::save()
{
    if (!is_write_mode_) return false;

    std::ofstream file(filepath_, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) return false;

    // Debug output
    std::cout << "BinarySerializer::save - Saving to " << filepath_ << std::endl;
    std::cout << "BinarySerializer::save - Number of entries: " << data_map_.size() << std::endl;
    
    // Print all keys being saved
    for (const auto& [key, value] : data_map_) {
        std::cout << "BinarySerializer::save - Key: " << key << ", Offset: " << value.first 
                  << ", Size: " << value.second << std::endl;
    }

    // Write the index table at the beginning of the file
    // First, write the number of entries
    int num_entries = data_map_.size();
    file.write(reinterpret_cast<const char*>(&num_entries), sizeof(int));

    // Then write each entry: key size, key, offset, size
    for (const auto& [key, value] : data_map_) {
        // Key size
        int key_size = key.size();
        file.write(reinterpret_cast<const char*>(&key_size), sizeof(int));

        // Key
        file.write(key.data(), key_size);

        // Offset and size
        file.write(reinterpret_cast<const char*>(&value.first), sizeof(size_t));
        file.write(reinterpret_cast<const char*>(&value.second), sizeof(size_t));
    }

    // Write data buffer
    file.write(data_buffer_.data(), data_buffer_.size());

    file.close();
    std::cout << "BinarySerializer::save - Save completed successfully" << std::endl;
    return true;
}

void BinarySerializer::write(const std::string& key, const std::string& value)
{
    // Record the current position
    size_t offset = data_buffer_.size();

    // First write the string length
    int length = value.size();
    const char* length_bytes = reinterpret_cast<const char*>(&length);
    data_buffer_.insert(data_buffer_.end(), length_bytes, length_bytes + sizeof(int));

    // Then write the string data
    data_buffer_.insert(data_buffer_.end(), value.begin(), value.end());

    // Store metadata - offset is relative to the beginning of the data buffer
    data_map_[key] = { offset, sizeof(int) + value.size() };
    
    std::cout << "BinarySerializer::write - Added string key: " << key 
              << ", value: " << value << ", offset: " << offset 
              << ", size: " << (sizeof(int) + value.size()) << std::endl;
}

void BinarySerializer::write(const std::string& key, int value)
{
    // Record the current position
    size_t offset = data_buffer_.size();

    // Convert to bytes (handle endianness if needed)
    int to_write = value;
    if (is_little_endian_ != system_is_little_endian()) {
        to_write = reverse_byte_order(value);
    }

    // Append to buffer
    const char* bytes = reinterpret_cast<const char*>(&to_write);
    data_buffer_.insert(data_buffer_.end(), bytes, bytes + sizeof(int));

    // Store metadata - offset is relative to the beginning of the data buffer
    data_map_[key] = { offset, sizeof(int) };
    
    std::cout << "BinarySerializer::write - Added int key: " << key 
              << ", value: " << value << ", offset: " << offset 
              << ", size: " << sizeof(int) << std::endl;
}

void BinarySerializer::write(const std::string& key, float value)
{
    // Record the current position
    size_t offset = data_buffer_.size();

    // Convert to bytes (handle endianness if needed)
    float to_write = value;
    if (is_little_endian_ != system_is_little_endian()) {
        to_write = reverse_byte_order(value);
    }

    // Append to buffer
    const char* bytes = reinterpret_cast<const char*>(&to_write);
    data_buffer_.insert(data_buffer_.end(), bytes, bytes + sizeof(float));

    // Store metadata - offset is relative to the beginning of the data buffer
    data_map_[key] = { offset, sizeof(float) };
    
    std::cout << "BinarySerializer::write - Added float key: " << key 
              << ", value: " << value << ", offset: " << offset 
              << ", size: " << sizeof(float) << std::endl;
}

void BinarySerializer::write(const std::string& key, bool value)
{
    // Record the current position
    size_t offset = data_buffer_.size();

    // Convert to bytes
    char byte = value ? 1 : 0;

    // Append to buffer
    data_buffer_.push_back(byte);

    // Store metadata - offset is relative to the beginning of the data buffer
    data_map_[key] = { offset, sizeof(char) };
    
    std::cout << "BinarySerializer::write - Added bool key: " << key 
              << ", value: " << (value ? "true" : "false") << ", offset: " << offset 
              << ", size: " << sizeof(char) << std::endl;
}

bool BinarySerializer::read(const std::string& key, std::string& value)
{
    std::cout << "BinarySerializer::read - Attempting to read string key: " << key << std::endl;
    
    auto it = data_map_.find(key);
    if (it == data_map_.end()) {
        std::cout << "BinarySerializer::read - Key not found in data_map_" << std::endl;
        return false;
    }
    
    if (it->second.first + sizeof(int) > data_buffer_.size()) {
        std::cout << "BinarySerializer::read - Offset + size exceeds buffer size" << std::endl;
        return false;
    }

    // Read string length
    size_t offset = it->second.first;
    int length;
    std::memcpy(&length, &data_buffer_[offset], sizeof(int));

    if (offset + sizeof(int) + length > data_buffer_.size()) {
        std::cout << "BinarySerializer::read - String length exceeds buffer size" << std::endl;
        return false;
    }

    // Read string data
    value.assign(data_buffer_.data() + offset + sizeof(int), length);
    
    std::cout << "BinarySerializer::read - Successfully read string value: " << value << std::endl;
    return true;
}

bool BinarySerializer::read(const std::string& key, int& value)
{
    std::cout << "BinarySerializer::read - Attempting to read int key: " << key << std::endl;
    
    auto it = data_map_.find(key);
    if (it == data_map_.end()) {
        std::cout << "BinarySerializer::read - Key not found in data_map_" << std::endl;
        return false;
    }
    
    if (it->second.first + it->second.second > data_buffer_.size()) {
        std::cout << "BinarySerializer::read - Offset + size exceeds buffer size" << std::endl;
        return false;
    }

    // Read bytes from buffer
    size_t offset = it->second.first;
    size_t size = it->second.second;

    if (size != sizeof(int)) {
        std::cout << "BinarySerializer::read - Size mismatch: expected " << sizeof(int) 
                  << ", got " << size << std::endl;
        return false;
    }

    // Copy bytes to value
    std::memcpy(&value, &data_buffer_[offset], size);

    // Handle endianness if needed
    if (is_little_endian_ != system_is_little_endian()) {
        value = reverse_byte_order(value);
    }

    std::cout << "BinarySerializer::read - Successfully read int value: " << value << std::endl;
    return true;
}

bool BinarySerializer::read(const std::string& key, float& value)
{
    std::cout << "BinarySerializer::read - Attempting to read float key: " << key << std::endl;
    
    auto it = data_map_.find(key);
    if (it == data_map_.end()) {
        std::cout << "BinarySerializer::read - Key not found in data_map_" << std::endl;
        return false;
    }
    
    if (it->second.first + it->second.second > data_buffer_.size()) {
        std::cout << "BinarySerializer::read - Offset + size exceeds buffer size" << std::endl;
        return false;
    }

    // Read bytes from buffer
    size_t offset = it->second.first;
    size_t size = it->second.second;

    if (size != sizeof(float)) {
        std::cout << "BinarySerializer::read - Size mismatch: expected " << sizeof(float) 
                  << ", got " << size << std::endl;
        return false;
    }

    // Copy bytes to value
    std::memcpy(&value, &data_buffer_[offset], size);

    // Handle endianness if needed
    if (is_little_endian_ != system_is_little_endian()) {
        value = reverse_byte_order(value);
    }

    std::cout << "BinarySerializer::read - Successfully read float value: " << value << std::endl;
    return true;
}

bool BinarySerializer::read(const std::string& key, bool& value)
{
    std::cout << "BinarySerializer::read - Attempting to read bool key: " << key << std::endl;
    
    auto it = data_map_.find(key);
    if (it == data_map_.end()) {
        std::cout << "BinarySerializer::read - Key not found in data_map_" << std::endl;
        return false;
    }
    
    if (it->second.first + it->second.second > data_buffer_.size()) {
        std::cout << "BinarySerializer::read - Offset + size exceeds buffer size" << std::endl;
        return false;
    }

    // Read bytes from buffer
    size_t offset = it->second.first;
    size_t size = it->second.second;

    if (size != sizeof(char)) {
        std::cout << "BinarySerializer::read - Size mismatch: expected " << sizeof(char) 
                  << ", got " << size << std::endl;
        return false;
    }

    // Read bool value
    char byte;
    std::memcpy(&byte, &data_buffer_[offset], size);
    value = byte != 0;

    std::cout << "BinarySerializer::read - Successfully read bool value: " << (value ? "true" : "false") << std::endl;
    return true;
}

} // namespace cge
