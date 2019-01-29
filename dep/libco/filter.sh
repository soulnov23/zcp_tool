 egrep -v "__func__|__PRETTY_|__FUNCTION|google::protobuf|_2eproto|std::__ioinit|::config|NextStackFrame|::default_instance_|_reflection_|_descriptor_|::def|FLAG__namespace_do_not_use_directly_use_DECLARE_string_instead|FLAG__namespace_do_not_use_directly_use_DECLAR|::_default_|^tcmalloc::|MemoryRegionMap|base::internal|HeapLeakChecker|TCMallocImplementation|std::tr1::|::protobuf_|Comm::CSysMetaConfig::ConvertClientConfigPath_Local\(\)::len|Comm::CSysMetaConfig::ConvertServerConfigPath\(\)::len|Comm::CSysMetaConfig::IsClientConfig\(\)::len|Comm::CSysMetaConfig::IsServerConfig\(\)::len|Comm::CSystemConfig::ConvertClientConfigPath_Local\(\)::len|Comm::CSystemConfig::ConvertClientConfigPath_Local\(\)::preIDCName|Comm::CEndpointConfig::CEndpointConfig\(\)::isInit|Comm::CLog::GetDefault\(\)::log|Comm::ConfigHashTable::HASH_SIZE|Comm::CPreforkSvr::|Comm::zk_mgr_client_key|Comm::zk_mgr_client_key_once|default_instance_|Comm::zk_mgr_client_key|Comm::CSystemConfig::ConvertClientConfigPath_Local()::preIDCName" |
 egrep -v "Oss::ReadDataFromPipe|Oss::ReadDataFromLog|Oss::WriteDataByLog|Oss::WriteDataByPipe|::_routine_|my_gettimeofday|new_gettimeofday" |
 egrep -v "Comm::|Oss::|coproc::" |
 egrep -v "gsTempName|gvLog|clsLogFile" |
 egrep -v "mimelinker::|CXmlReader|:CBaseVal|s_iEnableTOS"|
 egrep -v "tlvpickle::|TiXml"|
 egrep -v "MMUserAttrBasic::GetIdList\(\)::idlist"|
 egrep -v "clsLog::|clsSocket::|conv_p2\(\)::low_digits|conv_p2\(\)::upper_digits|CPCGetSelfIP|cpGetRandomInt|CXRAlarm|GetPinYinFromMB|GetPinYinFromMB|gsGetSysInfo|__cxxabiv1::|mime_linker_utils|_S_empty_rep_storage|ct_init_by_mem_pos::nOutLookLen"|
 egrep -v "AuthProCli::"|
 egrep -v "SKGlobalMem::"|
 egrep -v "ReadCallBack::"|
 egrep -v "m_AttrIndexMap"|
 egrep -v "MetaInfo"|
 egrep -v "gzrd_Lib_CPP_Version_ID"|
 egrep -v "OssAttr::"|
 egrep -v "zlib_"|
 egrep -v "tty_in|tty_new|tty_orig|tty_out|ui_openssl|use_monotonic|WIN32_method|xptable|xstandard"|
 sort|uniq -c
