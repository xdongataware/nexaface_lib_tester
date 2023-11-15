#pragma once
#include "aw_nexa_face.h"
#include "aw_nexa_face_errors.h"
#include "NexaFaceCommon.h"

#include <string>
#include <vector>
#include <memory>

#include <chrono>
#include <thread>

using namespace std;

class NexaFaceWrapper : public NexaFaceCommon
{
private:
    NexaFaceWrapper() {};

public:
    NexaFaceWrapper(const char* cache_name, const char*  cache_dir, int max_encounters, int results_size);
    ~NexaFaceWrapper();

    void ShowVersion();

    void Initialize();
    void ProcessAddSingleTemplate(const char* id, string template_data);
    void ProcessIdentify(string base64EncodedTemplateString);
    void ProcessDelete(string encounter_id_);
    void ProcessDrop();

private:
    shared_ptr< std::vector<unsigned char> > Base64Decode(std::string const& encoded_string);
    void WaitUntilDone();

    inline std::string GetErrorString(int code)
    {
        const aw_char_t* error_details;
        aw_nexa_face_get_error_details(code, &error_details);
        std::string string_details(error_details);
        aw_nexa_face_free((void*)error_details);
        return string_details;
    }

    static std::string GetString(const char* ptr)
    {
        std::string message;
        if (ptr)
        {
            message = ptr;
            aw_nexa_face_free((void*)ptr);
        }
        return message;
    }

    static void ThrowOnError(int error_code)
    {
        if (error_code != AW_NEXA_FACE_E_NO_ERRORS)
        {
            cout << "oops, got error: " << error_code << endl;
            const char* message_ptr;
            aw_nexa_face_get_error_details(error_code, &message_ptr);
            std::string message = GetString(message_ptr);

            cout << "error message: " << message << endl;
            throw std::runtime_error(message);
        }
    }

    bool CheckAndDisplayError(
        std::string jobType,
        std::string jobInfo,
        int errorCode);

    static void WINAPI OnCacheResultStatic(
        const char* jobId,
        int errorCode,
        const char* encounterId,
        const char* cacheName,
        void* privateData);
    void OnCacheResult(
        std::string jobId,
        int errorCode,
        std::string encounterId,
        std::string cacheName);

    static void WINAPI OnCacheBatchResultStatic(
        const char* jobId,
        int errorCode,
        const char** idList,
        size_t idListSize,
        const char* cacheName,
        void* privateData);
    void OnCacheBatchResult(
        std::string jobId,
        int errorCode,
        const char** idList,
        size_t idListSize,
        std::string cacheName);

    static void WINAPI OnCompareResultStatic(
        const char* jobId,
        int errorCode,
        aw_nexa_face_compare_result_t* result,
        void* privateData);
    void OnCompareResult(
        std::string jobId,
        int errorCode,
        aw_nexa_face_compare_result_t* result);

    static void WINAPI OnIdentifyResultStatic(
        const char* jobId,
        int errorCode,
        aw_nexa_face_candidate_list_t* candidateList,
        void* privateData);
    void OnIdentifyResult(
        std::string jobId,
        int errorCode,
        aw_nexa_face_candidate_list_t* candidateList);

    void CompleteOperation();

    aw_nexa_face_t* nexa_;                   // Library pointer
    bool add_new_operations_;                // A helper flag to not over-count operations
//    ofstream results_file_stream_;           // Writes CSV results file for longer operations like identify_all
    volatile size_t queued_operations_;      // Number of currently queued operations; used to limit memory usage
    volatile size_t completed_operations_;   // Number of completed operations
    volatile size_t total_operations_;       // Total number of expected operations, based on task chosen

    const char* cache_dir_;                  // Directory where cache created
    const char* cache_name_;                 // Name of cache
    int max_encounters_;                     // Maximum encounters in cache, or 0 to use defaults

    int results_size_;
};

