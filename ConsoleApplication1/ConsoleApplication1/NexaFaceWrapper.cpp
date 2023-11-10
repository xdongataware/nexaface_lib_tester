#include "stdafx.h"
#include "NexaFaceWrapper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;



NexaFaceWrapper::NexaFaceWrapper()
{
    max_encounters_ = 0;
    initialized_ = false;
    // identify_all_ = false;
    add_new_operations_ = true;
    queued_operations_ = 0;
    completed_operations_ = 0;
    total_operations_ = 0;
    cache_name_ = "cache";

    int error_code = aw_nexa_face_create(&nexa_);
    ThrowOnError(error_code);
}

NexaFaceWrapper::~NexaFaceWrapper()
{
}

void NexaFaceWrapper::ShowVersion()
{
    // cout << "  hello from version :)" << endl;
    cout << aw_nexa_face_get_version_string() << endl;

    /*
    */
}

void NexaFaceWrapper::Initialize() {
    /*
    if (!initialized_)
    {
        if (verbose_)
        {
            std::cout << "    NexaFaceWrapper::Initialize\n";
        }

        aw_nexa_face_cache_config_t* cache_config;
        int error_code = aw_nexa_face_create_cache_config(nexa_, cache_name_, &cache_config);
        ThrowOnError(error_code);

        error_code = aw_nexa_face_cache_config_enable_face(nexa_, cache_config, AW_NEXA_FACE_FACE_TYPE_VISIBLE_FRONTAL);
        ThrowOnError(error_code);

        aw_nexa_face_cache_type_t cache_type = AW_NEXA_FACE_CACHE_TYPE_RAM;
        algorithm_ = AW_NEXA_FACE_ALGORITHM_F500;

        if (sizeof(void*) == 4)
        {
            cache_type = AW_NEXA_FACE_CACHE_TYPE_DISK;
            algorithm_ = AW_NEXA_FACE_ALGORITHM_F500;
        }

        aw_nexa_face_enable_algorithm(nexa_, algorithm_);
        error_code = aw_nexa_face_cache_config_add_algorithm(nexa_, cache_config, algorithm_, cache_type);
        ThrowOnError(error_code);

        if (!cache_dir_.empty())
        {
            error_code = aw_nexa_face_cache_config_set_dir(nexa_, cache_config, cache_dir_.c_str());
            ThrowOnError(error_code);
        }

        if (max_encounters_ > 0)
        {
            error_code = aw_nexa_face_cache_config_set_max_encounters(nexa_, cache_config, max_encounters_);
            ThrowOnError(error_code);
        }
        if (sizeof(void*) == 4)
        {
            error_code = aw_nexa_face_set_algorithm_optimization_model(nexa_, algorithm_, "../redist/F500_mobile.bin");
            ThrowOnError(error_code);
        }
        else
        {
            error_code = aw_nexa_face_set_algorithm_optimization_model(nexa_, algorithm_, "../redist/F500_general.bin");
            ThrowOnError(error_code);
        }

        error_code = aw_nexa_face_add_cache(nexa_, cache_config);
        ThrowOnError(error_code);

        error_code = aw_nexa_face_set_worker_count(nexa_, 0);
        ThrowOnError(error_code);

        aw_nexa_face_cache_config_destroy(nexa_, cache_config);

        error_code = aw_nexa_face_set_export_callback(
            nexa_, &NexaFaceWrapper::OnExportStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_set_cache_result_callback(
            nexa_, &NexaFaceWrapper::OnCacheResultStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_set_cache_batch_result_callback(
            nexa_, &NexaFaceWrapper::OnCacheBatchResultStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_set_compare_result_callback(
            nexa_, &NexaFaceWrapper::OnCompareResultStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_set_verify_result_callback(
            nexa_, &NexaFaceWrapper::OnCompareResultStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_set_identify_result_callback(
            nexa_, &NexaFaceWrapper::OnIdentifyResultStatic, this);
        ThrowOnError(error_code);
        error_code = aw_nexa_face_initialize(nexa_);
        ThrowOnError(error_code);
    }
    initialized_ = true;
    */
}

