#pragma once
#include <windows.h>
#include <string>
#include <iostream>
#include <filesystem>
#include <json/json.h>
namespace fs = std::filesystem;

#include "PropertySystem.h"
#include "Log/LogSystem.h"

//###############学习#################
// 阅 - n遍
//####################################
inline fs::path getExecutablePath() {
#ifdef _WIN32
    wchar_t szFilePath[MAX_PATH + 1] = { 0 };
    GetModuleFileNameW(NULL, szFilePath, MAX_PATH);
    fs::path exePath(szFilePath);
    return exePath.parent_path();
#else
    // Linux/Mac: 使用 /proc/self/exe
    char buffer[PATH_MAX] = { 0 };
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        fs::path exePath(buffer);
        return exePath.parent_path();
    }
    return fs::current_path();  // 降级方案
#endif
}

#ifdef _WIN32
#include <windows.h>

// UTF-8 转 UTF-16 (Windows宽字符)
inline std::wstring Utf8ToWide(const std::string& utf8Str) {
    if (utf8Str.empty()) return L"";

    // 注意：使用 utf8Str.size() 而不是 -1，避免包含末尾的 '\0'
    int wideLen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), nullptr, 0);
    if (wideLen == 0) {
        LOG_ERROR_STREAM << u8"Utf8ToWide: MultiByteToWideChar Failed, error code:" << GetLastError() << std::endl;
        return L"";
    }

    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), (int)utf8Str.size(), &wideStr[0], wideLen);

    // 不需要额外去除末尾空字符，因为我们没有包含它
    return wideStr;
}

inline bool CheckIsDirectory(const std::string& path) {
    if (path.empty()) return false;

    try {
        std::wstring widePath = Utf8ToWide(path);

        DWORD attrib = GetFileAttributesW(widePath.c_str());
        if (attrib == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
        return (attrib & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << u8"检查目录错误 Failed to check directory: " << e.what() << std::endl;
        LOG_ERROR_STREAM << u8"Path: " << path << std::endl;
        return false;
    }
}

inline bool CheckFileExists(const std::string& path) {
    if (path.empty()) return false;

    try {
        std::wstring widePath = Utf8ToWide(path);

        DWORD attrib = GetFileAttributesW(widePath.c_str());
        if (attrib == INVALID_FILE_ATTRIBUTES) {
            return false;
        }
        return (attrib & FILE_ATTRIBUTE_DIRECTORY) == 0;  // 是文件（不是目录）
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << u8"检查目录错误 Failed to check directory: " << e.what() << std::endl;
        LOG_ERROR_STREAM << u8"Path: " << path << std::endl;
        return false;
    }
}

#else
// Linux/Mac 下直接使用 UTF-8
inline bool CheckIsDirectory(const std::string& path) {
    try {
        return fs::is_directory(path);
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << u8"检查目录错误 Failed to check directory: " << e.what() << std::endl;
        return false;
    }
}

inline bool CheckFileExists(const std::string& path) {
    try {
        return fs::exists(path) && !fs::is_directory(path);
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << u8"检查目录错误 Failed to check directory: " << e.what() << std::endl;
        return false;
    }
}
#endif

inline std::vector<fs::path> scanFontFiles(const std::string& folderPath, const std::string& extension) {
    std::vector<fs::path> fontFiles;

    // 在 scanFontFiles 函数开始处添加
    //std::cerr << u8"========== 开始扫描 ==========" << std::endl;
    //std::cerr << u8"目标文件夹: [" << folderPath << "]" << std::endl;

    // 检查路径字符串的原始字节
    //std::cerr << u8"路径字符串的十六进制: ";
    //for (unsigned char c : folderPath) {
    //    std::cerr << std::hex << (int)c << " ";
    //}
    //std::cerr << std::dec << std::endl;

    // 测试不同的路径格式
    std::error_code ec;

    // 1. 使用原始路径
    bool exists1 = fs::exists(folderPath, ec);
    LOG_INFO_STREAM << u8"1. fs::exists(原始路径 original path): " << exists1;
    if (ec) LOG_ERROR_STREAM << u8", Failed: " << ec.message();
    //LOG_ERROR_STREAM << std::endl;

    // 2. 转换为宽字符串
    std::wstring widePath = Utf8ToWide(folderPath);
    bool exists2 = fs::exists(widePath, ec);
    LOG_INFO_STREAM << u8"2. fs::exists(宽字符串 wstring): " << exists2;
    if (ec) LOG_ERROR_STREAM << u8", Failed: " << ec.message();
    //LOG_ERROR_STREAM << std::endl;

    bool IsDirectory = CheckIsDirectory(folderPath);
    // 检查路径是否存在且是目录
    if (!IsDirectory) {
        LOG_ERROR_STREAM << u8"Error:目标路径不是目录或不存在 target path does not exist or is not a directory";
        return fontFiles;  // 返回空列表
    }

    // 修改点：在 Windows 下使用宽字符串版本的目录迭代器
#ifdef _WIN32
    LOG_INFO_STREAM << u8"使用宽字符串版本遍历目录 Using wide string version to traverse directory...";
    try {
        for (const auto& entry : fs::directory_iterator(widePath)) {
            // 只处理文件，忽略子目录
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                // 转换为小写
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

                // 检查是否是字体文件
                if (ext == extension) {
                    fontFiles.push_back(entry.path());
                    LOG_INFO_STREAM << u8"找到字体文件 Font file found: " << entry.path().filename().string();
                }
            }
        }
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << u8"扫描目录异常 Exception while scanning directory: " << e.what();
        return fontFiles;
    }
#else
    LOG_INFO_STREAM << u8"使用 UTF-8 版本遍历目录 Using UTF-8 version for directory traversal...";
    for (const auto& entry : fs::directory_iterator(folderPath)) {
        // 只处理文件，忽略子目录
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            // 转换为小写
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

            // 检查是否是字体文件
            if (ext == extension) {
                fontFiles.push_back(entry.path());
            }
        }
    }
#endif

    LOG_INFO_STREAM << u8"Found " << fontFiles.size() << u8" font files";
    return fontFiles;
}

inline std::string AnsiToUtf8(const std::string& ansiStr) {
    if (ansiStr.empty()) return "";

    // ANSI 转宽字符
    int wideLen = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, nullptr, 0);
    std::wstring wideStr(wideLen, 0);
    MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, &wideStr[0], wideLen);

    // 宽字符转 UTF-8
    int utf8Len = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8Str(utf8Len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), -1, &utf8Str[0], utf8Len, nullptr, nullptr);

    // 移除末尾空字符
    if (!utf8Str.empty() && utf8Str.back() == '\0') {
        utf8Str.pop_back();
    }
    return utf8Str;
}


inline std::string getFileName(const std::string& fullPath) {
    // 查找最后一个分隔符
    size_t lastSlash = fullPath.find_last_of("\\");
    if (lastSlash != std::string::npos) {
        return fullPath.substr(lastSlash + 1);
    }
    return fullPath;
}

// 跨平台字体加载辅助函数
inline bool AddFontFromFileTTF(ImGuiIO& io, const fs::path& fontPath, float fontSize, const ImWchar* glyphRanges = nullptr) {
    // ImGui 在所有平台都使用 UTF-8 字符串
    std::string utf8Path = fontPath.u8string();
    return io.Fonts->AddFontFromFileTTF(utf8Path.c_str(), fontSize, nullptr, glyphRanges) != nullptr;
}

inline bool fontLoadTool(std::string path, ImGuiIO& io, M_Property::PropertyContainer& props) {
    props.AddGroup("FontsData");
    std::vector<fs::path> files = scanFontFiles(path, ".ttf");
    if (files.empty()) {
        io.Fonts->AddFontDefault();
        io.Fonts->Build();
        ImGui::SFML::UpdateFontTexture();
        return false;
    }

    LOG_INFO_STREAM << u8"文件总量 Total files:" << files.size();

    int FontCount = 0;
    io.Fonts->Clear();

    // 加载默认字体
    //std::string storedPath = props.GetValue<std::string>("MainData", "DefaultFont_SimHei_Path");
    //std::string utf8Path = AnsiToUtf8(storedPath);  // 转换编码
    //fs::path DefaulFontPath = fs::path(utf8Path);
    fs::path defaultFontPath = getExecutablePath() / "font" / "DefaultFont" / "SimHei.ttf";
    props.AddProperty("FontsData", M_Property::Property(defaultFontPath.u8string(), "SimHei", defaultFontPath.u8string()));

    LOG_INFO_STREAM << u8"默认字体路径（原始）Raw default font path: [" << defaultFontPath.u8string() << "]";
    LOG_INFO_STREAM << u8"默认字体路径长度 Default font path length: " << defaultFontPath.u8string().length();

    ImFontConfig simhei_config;
    simhei_config.OversampleH = 1;
    simhei_config.OversampleV = 1;
    simhei_config.PixelSnapH = true;
    // 使用辅助函数加载
    //AddFontFromFileTTF(io, defaultFontPath, 16.0f, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->AddFontFromFileTTF(defaultFontPath.u8string().c_str(),
        props.GetValue<int>("MainUIdata", "FontScale"),
        &simhei_config,                   // 字体配置
        io.Fonts->GetGlyphRangesChineseFull()  // 中文字符集
    );

    io.Fonts->Build();
    ImGui::SFML::UpdateFontTexture();

    FontCount++;
    for (fs::path& file : files)
    {
        LOG_INFO_STREAM << " ";
        LOG_INFO_STREAM << u8"path:" << file.u8string();
        std::string fileName = getFileName(file.u8string());
        LOG_INFO_STREAM << u8"file name:" << fileName;

        props.AddProperty("FontsData", M_Property::Property(file.u8string(), fileName, file.u8string()));
        FontCount++;
    }

    LOG_INFO_STREAM << u8"扫描完成 Scan complete! Found" << FontCount << u8" FontCount," << u8"loaded  " << io.Fonts->Fonts.Size << u8" fonts";

    return true;
}

inline bool setFont(std::string DefaultFontPath, std::string path, float size = 16.0f) {
    LOG_INFO_STREAM << u8"字体加载:";
    LOG_INFO_STREAM << u8"默认字体路径 Default font path: " << DefaultFontPath;
    LOG_INFO_STREAM << u8"默认字体路径长度 Default font path length: " << DefaultFontPath.length();

    LOG_INFO_STREAM << u8"目标字体路径 Target font path: " << path;
    LOG_INFO_STREAM << u8"目标字体路径长度 Target font path length: " << path.length();
    
    bool DefaultFontFileExists = CheckFileExists(DefaultFontPath);
    bool FontFileExists = CheckFileExists(path);
    if (!(DefaultFontFileExists && FontFileExists)) {
        LOG_ERROR_STREAM << u8"目标字体文件不存在 Target font file does not exist:";
        if (!DefaultFontFileExists){ LOG_ERROR_STREAM << u8" 默认字体不存在 Default font does not exist"; }
        if (!FontFileExists) { LOG_ERROR_STREAM << u8" 要设置的字体不存在 The font to be set does not exist"; }
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();

    ImFontConfig simhei_config;
    simhei_config.OversampleH = 1;
    simhei_config.OversampleV = 1;
    simhei_config.PixelSnapH = true;
    ImFont* defaultFont = io.Fonts->AddFontFromFileTTF(DefaultFontPath.c_str(),
        size,
        &simhei_config,                   // 字体配置
        io.Fonts->GetGlyphRangesChineseFull()  // 中文字符集
    );

    if (!defaultFont) {
        LOG_ERROR_STREAM << u8"默认字体加载失败 Default font failed to load" << std::endl;
        io.Fonts->AddFontDefault();
    }

    ImFont* font = io.Fonts->AddFontFromFileTTF(path.c_str(),
        size,
        nullptr,                   // 字体配置
        io.Fonts->GetGlyphRangesChineseFull()  // 中文字符集
    );

    io.Fonts->Build();
    ImGui::SFML::UpdateFontTexture();
    return font != nullptr;
}

// 保存JSON文件（支持Unicode路径）
inline bool saveJsonFile(const std::string& filename, const Json::Value& root) {
    // 确保目录存在
    fs::path filePath(filename);
    fs::path dirPath = filePath.parent_path();
    if (!dirPath.empty() && !fs::exists(dirPath)) {
        if (!fs::create_directories(dirPath)) {
            LOG_ERROR_STREAM << "无法创建目录: " << dirPath.u8string();
            return false;
        }
    }

    // 使用宽字符版本打开文件（支持中文路径）
#ifdef _WIN32
    std::ofstream file;
    file.open(fs::path(filename).wstring(), std::ios::out | std::ios::binary);
#else
    std::ofstream file(filename, std::ios::out | std::ios::binary);
#endif

    if (!file.is_open()) {
        LOG_ERROR_STREAM << "无法打开文件进行写入: " << filename;
        return false;
    }

    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    writerBuilder["commentStyle"] = "None";

    try {
        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
        writer->write(root, &file);
        file.close();
        return true;
    }
    catch (const std::exception& e) {
        LOG_ERROR_STREAM << "保存JSON文件时发生异常: " << e.what();
        file.close();
        return false;
    }
}

// 读取JSON文件（支持Unicode路径）
inline bool readJsonFile(const std::string& filename, Json::Value& root) {
    // 使用宽字符版本打开文件
#ifdef _WIN32
    std::ifstream file(fs::path(filename).wstring(), std::ios::binary);
#else
    std::ifstream file(filename, std::ios::binary);
#endif

    // 文件不存在，创建默认配置
    if (!file.is_open()) {
        LOG_WARNING_STREAM << "配置文件不存在，将创建默认配置: " << filename;

        // 确保目录存在
        fs::path filePath(filename);
        fs::path dirPath = filePath.parent_path();
        if (!dirPath.empty() && !fs::exists(dirPath)) {
            fs::create_directories(dirPath);
        }

        // 创建带默认值的配置
        Json::Value defaultRoot;

        // 获取程序目录并设置默认字体路径
        fs::path exePath = getExecutablePath();
        fs::path defaultFontPath = exePath / "font" / "DefaultFont" / "SimHei.ttf";

        if (saveJsonFile(filename, defaultRoot)) {
            root = defaultRoot;
            return true;
        }
        return false;
    }

    // 读取并解析文件
    Json::CharReaderBuilder readerBuilder;
    std::string parseErrors;

    // 读取整个文件内容
    std::string fileContent;
    file.seekg(0, std::ios::end);
    fileContent.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&fileContent[0], fileContent.size());
    file.close();

    // 解析JSON
    std::istringstream contentStream(fileContent);
    bool success = Json::parseFromStream(readerBuilder, contentStream, &root, &parseErrors);

    if (!success) {
        LOG_ERROR_STREAM << "JSON解析失败: " << parseErrors << " 文件: " << filename;

        // 尝试备份损坏的文件
        std::string backupFile = filename + ".backup";
        try {
            fs::copy(filename, backupFile, fs::copy_options::overwrite_existing);
            LOG_WARNING_STREAM << "已备份损坏的配置文件到: " << backupFile;
        }
        catch (...) {}

        return false;
    }

    return true;
}

// 生成带时间戳的文件名
static std::string generateTimestampFilename(std::string extension) {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;

#ifdef _WIN32
    struct tm buf;
    localtime_s(&buf, &now_time_t);
    ss << std::put_time(&buf, "%Y%m%d_%H%M%S");
#else
    struct tm buf;
    localtime_r(&now_time_t, &buf);
    ss << std::put_time(&buf, "%Y%m%d_%H%M%S");
#endif

    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    ss << extension;

    return ss.str();
}

// 打开文件夹
static void OpenFolderInExplorer(const std::string& path) {
#ifdef _WIN32
    // 正确转换 UTF-8 到 UTF-16 (Windows 宽字符)
    std::wstring wpath = Utf8ToWide(path);
    ShellExecuteW(NULL, L"open", L"explorer", wpath.c_str(), NULL, SW_SHOW);
#else
    std::string command = "xdg-open \"" + path + "\"";
    system(command.c_str());
#endif
}
