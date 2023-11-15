#include "stdafx.h"
#include "NexaFaceWrapper.h"

#include <stdexcept> // For standard exception classes
#include <iostream>  // For std::cout


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";



NexaFaceWrapper::NexaFaceWrapper(const char* cache_name, const char*  cache_dir, int max_encounters, int results_size)
    : cache_name_(cache_name)
    , cache_dir_(cache_dir)
    , max_encounters_(max_encounters)
    , results_size_(results_size)
{
    // identify_all_ = false;
    add_new_operations_ = true;
    queued_operations_ = 0;
    completed_operations_ = 0;
    total_operations_ = 0;

    int error_code = aw_nexa_face_create(&nexa_);
    ThrowOnError(error_code);
}

NexaFaceWrapper::~NexaFaceWrapper()
{
}

void NexaFaceWrapper::ShowVersion()
{
    cout << aw_nexa_face_get_version_string() << endl;
}

void NexaFaceWrapper::OnCacheResultStatic(
    const char* jobId,
    int errorCode,
    const char* encounterId,
    const char* cacheName,
    void* privateData)
{
    ((NexaFaceWrapper*)privateData)->OnCacheResult(
        jobId, errorCode, encounterId, cacheName);
}

void NexaFaceWrapper::OnCacheResult(
    std::string jobId,
    int errorCode,
    std::string encounterId,
    std::string cacheName)
{
    size_t jobTypeIndex = jobId.find('_');
    std::string jobType = jobId.substr(0, jobTypeIndex);
    std::string jobInfo = jobId.substr(jobTypeIndex + 1);
    std::cout << "OnCacheResult, jobId:" << jobId << " errorCode: " << errorCode << " jobType: " << jobType << " jobInfo: " << jobInfo << "\n";
    if (!CheckAndDisplayError(jobType, jobInfo, errorCode))
    {
        if (jobType == "add")
        {
            std::cout << "Added " << jobInfo << " with ID: " << encounterId << ".\n";
        }
        else if (jobType == "update")
        {
            std::cout << "Updated " << jobInfo << ".\n";
        }
        else if (jobType == "delete")
        {
            std::cout << "Deleted " << jobInfo << ".\n";
        }
        else if (jobType == "drop")
        {
            std::cout << "Drop operation completed successfully.\n";
        }
        else
        {
            std::cout << "Unknown cache job type: " << jobType << ".\n";
        }
    }
    std::cout << "completing operation ..." << "\n";
    CompleteOperation();
}


void WINAPI NexaFaceWrapper::OnCacheBatchResultStatic(
    const char* jobId,
    int errorCode,
    const char** idList,
    size_t idListSize,
    const char* cacheName,
    void* privateData)
{
    ((NexaFaceWrapper*)privateData)->OnCacheBatchResult(
        jobId, errorCode, idList, idListSize, cacheName);
}

void NexaFaceWrapper::OnCacheBatchResult(
    std::string jobId,
    int errorCode,
    const char** idList,
    size_t idListSize,
    std::string cacheName)
{
    size_t jobTypeIndex = jobId.find('_');
    std::string jobType = jobId.substr(0, jobTypeIndex);
    std::string jobInfo = jobId.substr(jobTypeIndex + 1);
    if (!CheckAndDisplayError(jobType, jobInfo, errorCode))
    {
        for (size_t i = 0; i<idListSize; i++)
        {
            const char* encounterId = idList[i];
            std::cout << "Added internal ID: " << encounterId << "\n";
        }
    }
    CompleteOperation();
}

bool NexaFaceWrapper::CheckAndDisplayError(
    std::string jobType,
    std::string jobInfo,
    int errorCode)
{
    if (errorCode != AW_NEXA_FACE_E_NO_ERRORS)
    {
        std::string extraInfo = "";
        if (jobInfo.size() > 0)
            extraInfo = " for " + jobInfo;
        std::cout << "Operation " << jobType << extraInfo << " failed.\n";
        const char* message;
        aw_nexa_face_get_error_details(errorCode, &message);
        std::cout << "Error: " << GetString(message) << "\n";
        return true;
    }
    return false;
}

void NexaFaceWrapper::CompleteOperation()
{
    completed_operations_++;
    cout << "completed operation: " << completed_operations_ << endl;
}


void NexaFaceWrapper::Initialize() {
        cout << "NexaFaceWrapper::Initialize\n";

        aw_nexa_face_cache_config_t* cache_config;
        cout << "aw_nexa_face_create_cache_config" << endl;
        int error_code = aw_nexa_face_create_cache_config(nexa_, cache_name_, &cache_config);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_cache_config_enable_face" << std::endl;
        error_code = aw_nexa_face_cache_config_enable_face(nexa_, cache_config, AW_NEXA_FACE_FACE_TYPE_VISIBLE_FRONTAL);
        ThrowOnError(error_code);

        aw_nexa_face_cache_type_t cache_type = AW_NEXA_FACE_CACHE_TYPE_RAM;
        aw_nexa_face_algorithm_t algorithm_ = AW_NEXA_FACE_ALGORITHM_F500;

        if (sizeof(void*) == 4)
        {
            cache_type = AW_NEXA_FACE_CACHE_TYPE_DISK;
            algorithm_ = AW_NEXA_FACE_ALGORITHM_F500;
            throw std::runtime_error("unexpected: sizeof(void*) == 4");
        }

        std::cout << "aw_nexa_face_enable_algorithm: " << algorithm_ << std::endl;
        aw_nexa_face_enable_algorithm(nexa_, algorithm_);

        std::cout << "aw_nexa_face_cache_config_add_algorithm, cache_type: " << cache_type << std::endl;
        error_code = aw_nexa_face_cache_config_add_algorithm(nexa_, cache_config, algorithm_, cache_type);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_cache_config_set_dir" << std::endl;
        error_code = aw_nexa_face_cache_config_set_dir(nexa_, cache_config, cache_dir_);
        ThrowOnError(error_code);

        if (max_encounters_ > 0)
        {
            std::cout << "aw_nexa_face_cache_config_set_max_encounters" << std::endl;
            error_code = aw_nexa_face_cache_config_set_max_encounters(nexa_, cache_config, max_encounters_);
            ThrowOnError(error_code);
        }

        std::cout << "aw_nexa_face_set_algorithm_optimization_model" << std::endl;
        error_code = aw_nexa_face_set_algorithm_optimization_model(nexa_, algorithm_, "../redist/F500_general.bin");
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_add_cache" << std::endl;
        error_code = aw_nexa_face_add_cache(nexa_, cache_config);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_set_worker_count" << std::endl;
        error_code = aw_nexa_face_set_worker_count(nexa_, 0);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_cache_config_destroy" << std::endl;
        aw_nexa_face_cache_config_destroy(nexa_, cache_config);


        /*
        error_code = aw_nexa_face_set_export_callback(
        nexa_, &NexaFaceWrapper::OnExportStatic, this);
        ThrowOnError(error_code);

        */

        std::cout << "aw_nexa_face_set_cache_result_callback" << std::endl;
        error_code = aw_nexa_face_set_cache_result_callback(
            nexa_, &NexaFaceWrapper::OnCacheResultStatic, this);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_set_cache_batch_result_callback" << std::endl;
        error_code = aw_nexa_face_set_cache_batch_result_callback(
            nexa_, &NexaFaceWrapper::OnCacheBatchResultStatic, this);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_set_compare_result_callback" << std::endl;
        error_code = aw_nexa_face_set_compare_result_callback(
            nexa_, &NexaFaceWrapper::OnCompareResultStatic, this);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_set_verify_result_callback" << std::endl;
        error_code = aw_nexa_face_set_verify_result_callback(
            nexa_, &NexaFaceWrapper::OnCompareResultStatic, this);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_set_identify_result_callback" << std::endl;
        error_code = aw_nexa_face_set_identify_result_callback(
            nexa_, &NexaFaceWrapper::OnIdentifyResultStatic, this);
        ThrowOnError(error_code);

        std::cout << "aw_nexa_face_initialize" << std::endl;
        error_code = aw_nexa_face_initialize(nexa_);
        ThrowOnError(error_code);

        cout << "  initialization completed" << endl;
}


void NexaFaceWrapper::ProcessAddSingleTemplate(const char* id, string base64EncodedTemplateString)
{
    if (add_new_operations_)
        total_operations_++;

    cout << "creating encounter" << endl;
    aw_nexa_face_encounter_t* encounter;
    int error_code = aw_nexa_face_create_encounter(nexa_, &encounter);
    ThrowOnError(error_code);

    auto data = Base64Decode(base64EncodedTemplateString);
    // std::vector<unsigned char> template_data(templateString.begin(), templateString.end());

    cout << "importing from template" << endl;
    error_code = aw_nexa_face_encounter_import_from_template(
        nexa_,
        encounter,
        //        &template_data.front(),
        //        template_data.size());
        data->data(), 
        data->size());
    ThrowOnError(error_code);

    cout << "set encounter id: " << id << endl;
    error_code = aw_nexa_face_encounter_set_id(
        nexa_,
        encounter,
        id);
    ThrowOnError(error_code);

    std::string job_id = "add_" + string(id);

    cout << "add to cache" << endl;
    error_code = aw_nexa_face_cache_add(nexa_, job_id.c_str(), encounter, cache_name_);
    ThrowOnError(error_code);
    aw_nexa_face_encounter_destroy(nexa_, encounter);

    WaitUntilDone();
    cout << "ProcessAddSingleTemplate completed" << endl;
}


shared_ptr<std::vector<unsigned char> > NexaFaceWrapper::Base64Decode(const std::string& encoded_string)
{
    std::string binary_string;

    std::vector<int> T(256, -1);
    for (int i = 0; i<64; i++) T[base64_chars[i]] = i;

    int val = 0, valb = -8;
    for (char s : encoded_string)
    {
        unsigned char c = (unsigned char)s;
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0)
        {
            binary_string.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }

    // Convert binary string to a buffer.
    shared_ptr<std::vector<unsigned char> > binary_buffer(
        new std::vector<unsigned char>(binary_string.begin(), binary_string.end()));

    return binary_buffer;
}

void NexaFaceWrapper::OnIdentifyResultStatic(
    const char* jobId,
    int errorCode,
    aw_nexa_face_candidate_list_t* candidateList,
    void* privateData)
{
    ((NexaFaceWrapper*)privateData)->OnIdentifyResult(
        jobId, errorCode, candidateList);
}

void NexaFaceWrapper::OnIdentifyResult(
    std::string jobId,
    int errorCode,
    aw_nexa_face_candidate_list_t* candidateList)
{
    size_t jobTypeIndex = jobId.find('_');
    std::string jobType = jobId.substr(0, jobTypeIndex);
    std::string jobInfo = jobId.substr(jobTypeIndex + 1);
    std::cout << "OnIdentifyResult ..." << "\n";
    if (!CheckAndDisplayError(jobType, jobInfo, errorCode))
    {
        size_t final_results_size = 0;
        aw_nexa_face_identify_result_t** results;
        std::cout << "aw_nexa_face_candidate_list_results" << "\n";
        aw_nexa_face_candidate_list_results(
            nexa_,
            candidateList,
            results_size_,
            &results,
            &final_results_size);
        if (results_size_ > 0 && final_results_size > results_size_) {
            final_results_size = results_size_;
        }
        std::cout << "results_size_: " << results_size_ << "\n";
        for (size_t index = 0; index<final_results_size; index++)
        {
            aw_nexa_face_identify_result_t* identify_result = results[index];
            std::cout << "Result #:" << index << ":\n";
            const aw_char_t* encounter_id_ptr;
            aw_nexa_face_identify_result_get_encounter_id(nexa_, identify_result, &encounter_id_ptr);
            if (encounter_id_ptr)
            {
                std::cout << "  id: " << encounter_id_ptr << "\n";
            }
            float score;
            aw_nexa_face_identify_result_get_score(nexa_, identify_result, &score);
            float percent;
            aw_nexa_face_get_score_as_percent(score, &percent);
            std::cout << " score: " << score << " - percent: " << percent << "\n";
            aw_nexa_face_identify_result_destroy(nexa_, results[index]);
        }
    }
    std::cout << "aw_nexa_face_candidate_list_destroy" << "\n";
    aw_nexa_face_candidate_list_destroy(nexa_, candidateList);
    CompleteOperation();
}



void NexaFaceWrapper::ProcessIdentify(string base64EncodedTemplateString)
{
    std::cout << "    v::ProcessIdentify\n";

    if (add_new_operations_)
        total_operations_++;

    cout << "creating probe encounter" << endl;
    aw_nexa_face_encounter_t* encounter;
    int error_code = aw_nexa_face_create_encounter(nexa_, &encounter);
    ThrowOnError(error_code);

    auto data = Base64Decode(base64EncodedTemplateString);
    // std::vector<unsigned char> template_data(templateString.begin(), templateString.end());

    cout << "importing from template" << endl;
    error_code = aw_nexa_face_encounter_import_from_template(
        nexa_,
        encounter,
        //        &template_data.front(),
        //        template_data.size());
        data->data(),
        data->size());
    ThrowOnError(error_code);

    const char* id = "someone";
    cout << "aw_nexa_face_encounter_set_id id: " << id << endl;
    error_code = aw_nexa_face_encounter_set_id(
        nexa_,
        encounter,
        id);
    ThrowOnError(error_code);

    aw_nexa_face_workflow_t* workflow;
    cout << "aw_nexa_face_create_workflow" << endl;
    error_code = aw_nexa_face_create_workflow(nexa_, &workflow);
    ThrowOnError(error_code);

    std::vector<aw_nexa_face_face_type_t> faces;
    faces.push_back(AW_NEXA_FACE_FACE_TYPE_VISIBLE_FRONTAL);

    aw_nexa_face_cache_type_t cache_type = AW_NEXA_FACE_CACHE_TYPE_RAM;
    aw_nexa_face_algorithm_t algorithm_ = AW_NEXA_FACE_ALGORITHM_F500;

    cout << "aw_nexa_face_workflow_set_comparator" << endl;
    error_code = aw_nexa_face_workflow_set_comparator(
        nexa_,
        workflow,
        algorithm_,
        &faces.front(),
        faces.size());
    ThrowOnError(error_code);

    cout << "aw_nexa_face_enable_algorithm" << endl;
    aw_nexa_face_enable_algorithm(nexa_, algorithm_);
    ThrowOnError(error_code);

    cout << "aw_nexa_face_identify" << endl;
    error_code = aw_nexa_face_identify(nexa_, "identify_", encounter, workflow, cache_name_);
    ThrowOnError(error_code);

    cout << "aw_nexa_face_encounter_destroy" << endl;
    aw_nexa_face_encounter_destroy(nexa_, encounter);
    aw_nexa_face_workflow_destroy(nexa_, workflow);

    WaitUntilDone();
    cout << "ProcessIdentify completed" << endl;
}

void NexaFaceWrapper::OnCompareResultStatic(
    const char* jobId,
    int errorCode,
    aw_nexa_face_compare_result_t* result,
    void* privateData)
{
    ((NexaFaceWrapper*)privateData)->OnCompareResult(
        jobId, errorCode, result);
}

void NexaFaceWrapper::OnCompareResult(std::string jobId, int errorCode, aw_nexa_face_compare_result_t* result)
{
    size_t jobTypeIndex = jobId.find('_');
    std::string jobType = jobId.substr(0, jobTypeIndex);
    std::string jobInfo = jobId.substr(jobTypeIndex + 1);
    if (!CheckAndDisplayError(jobType, jobInfo, errorCode))
    {
        float score;
        aw_nexa_face_compare_result_get_score(nexa_, result, &score);
        float percent;
        aw_nexa_face_get_score_as_percent(score, &percent);
        std::cout << jobType << " - score: " << score << " - percent: " << percent << "\n";
    }
    aw_nexa_face_compare_result_destroy(nexa_, result);
    CompleteOperation();
}

void NexaFaceWrapper::WaitUntilDone()
{
    while (completed_operations_ != total_operations_) {
        std::cout << "completed_operations_: " << completed_operations_ << " total_operations_: " << total_operations_ << "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "completed_operations_: " << completed_operations_ << " total_operations_: " << total_operations_ << "\n";
    std::cout << "   ***************  WAIT IS OVER ***************** " << "\n";
}

void NexaFaceWrapper::ProcessDelete(string encounter_id_)
{
    std::cout << "    NexaFaceDemo::ProcessDelete\n";
    if (add_new_operations_)
        total_operations_++;
    string job_id = string("delete_") + encounter_id_;
    int error_code = aw_nexa_face_cache_delete(nexa_, job_id.c_str(), encounter_id_.c_str(), cache_name_);
    ThrowOnError(error_code);
    WaitUntilDone();
}

void NexaFaceWrapper::ProcessDrop()
{
    std::cout << "    NexaFaceDemo::ProcessDrop\n";
    total_operations_++;
    int error_code = aw_nexa_face_cache_drop(nexa_, "drop_", cache_name_);
    ThrowOnError(error_code);
    WaitUntilDone();
    // Drop always goes before other operations, so reset counts.
    completed_operations_ = 0;
    total_operations_ = 0;
}
