#pragma once
#include "aw_nexa_face.h"
#include "aw_nexa_face_errors.h"
#include "NexaFaceCommon.h"

class NexaFaceWrapper : public NexaFaceCommon
{
public:
    NexaFaceWrapper();
    ~NexaFaceWrapper();

    virtual void ShowVersion();

private:

    void Initialize();

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
            const char* message_ptr;
            aw_nexa_face_get_error_details(error_code, &message_ptr);
            std::string message = GetString(message_ptr);
            throw std::runtime_error(message);
        }
    }

    aw_nexa_face_t* nexa_;                   // Library pointer
    std::string cache_dir_;                  // Directory where cache created
    const char* cache_name_;                 // Name of cache
    aw_nexa_face_algorithm_t algorithm_;    // Algorithm to use
    int max_encounters_;                     // Maximum encounters in cache, or 0 to use defaults
//    bool identify_all_;                      // True if conducting a probe directory : gallery directory seach
    bool initialized_;                       // True if library successfully initialized
    bool add_new_operations_;                // A helper flag to not over-count operations
//    ofstream results_file_stream_;           // Writes CSV results file for longer operations like identify_all
    volatile size_t queued_operations_;      // Number of currently queued operations; used to limit memory usage
    volatile size_t completed_operations_;   // Number of completed operations
    volatile size_t total_operations_;       // Total number of expected operations, based on task chosen

};

