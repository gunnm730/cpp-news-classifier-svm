cmake_minimum_required(VERSION 3.15)
project(cpp-news-classifier-svm)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8 /W4)
else()
    add_compile_options(-Wall -Wextra -pedantic -Wno-unused-parameter)
endif()

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set(SRC_DIR ${PROJECT_ROOT}/src)
set(VENDOR_DIR ${PROJECT_ROOT}/vendor)

set(HTTPLIB_INCLUDE ${VENDOR_DIR}/cpp-httplib)
set(JSON_INCLUDE ${VENDOR_DIR}/json/include)
set(CPPJIEBA_INCLUDE ${VENDOR_DIR}/cppjieba/include)
set(LIMONP_INCLUDE ${VENDOR_DIR}/cppjieba/deps/limonp/include)

set(MINIZ_SOURCES
    ${VENDOR_DIR}/miniz/miniz.c
    ${VENDOR_DIR}/miniz/miniz_zip.c
    ${VENDOR_DIR}/miniz/miniz_tinfl.c
    ${VENDOR_DIR}/miniz/miniz_tdef.c
)

set(COMMON_SOURCES
    ${SRC_DIR}/common/types.hpp
    ${SRC_DIR}/common/status_code.hpp
)

set(PREPROCESSOR_SOURCES
    ${SRC_DIR}/preprocessor/text_preprocessor.cpp
    ${SRC_DIR}/preprocessor/text_preprocessor.hpp
    ${SRC_DIR}/preprocessor/tokenizer.cpp
    ${SRC_DIR}/preprocessor/tokenizer.hpp
    ${SRC_DIR}/preprocessor/cleaner.cpp
    ${SRC_DIR}/preprocessor/cleaner.hpp
)

set(CORE_SOURCES
    ${SRC_DIR}/core/svm.cpp
    ${SRC_DIR}/core/svm.hpp
    ${SRC_DIR}/core/Tfidfvectorizer.cpp
    ${SRC_DIR}/core/Tfidfvectorizer.hpp
    ${SRC_DIR}/core/Preprocessor.cpp
    ${SRC_DIR}/core/Preprocessor.hpp
    ${SRC_DIR}/core/types.hpp
    ${SRC_DIR}/core/onetoall.cpp
    ${SRC_DIR}/core/onetoall.hpp
)

set(MY_SVM_SOURCES
    ${SRC_DIR}/my_svm/include/svm.cpp
    ${SRC_DIR}/my_svm/include/svm.hpp
    ${SRC_DIR}/my_svm/include/tfidf_vectorizer.cpp
    ${SRC_DIR}/my_svm/include/tfidf_vectorizer.hpp
    ${SRC_DIR}/my_svm/include/classifier.cpp
    ${SRC_DIR}/my_svm/include/classifier.hpp
    ${SRC_DIR}/my_svm/include/types.hpp
    ${SRC_DIR}/my_svm/include/model_serializer.cpp
    ${SRC_DIR}/my_svm/include/model_serializer.hpp
)

set(PARSER_SOURCES
    ${SRC_DIR}/parser/i_parser.hpp
    ${SRC_DIR}/parser/txt_parser.cpp
    ${SRC_DIR}/parser/txt_parser.hpp
    ${SRC_DIR}/parser/docx_parser.cpp
    ${SRC_DIR}/parser/docx_parser.hpp
    ${SRC_DIR}/parser/zip_parser.cpp
    ${SRC_DIR}/parser/zip_parser.hpp
    ${SRC_DIR}/parser/parser_factory.cpp
    ${SRC_DIR}/parser/parser_factory.hpp
)

set(PROXY_SOURCES
    ${SRC_DIR}/proxy/i_model_proxy.hpp
    ${SRC_DIR}/proxy/svm_predictor.cpp
    ${SRC_DIR}/proxy/svm_predictor.hpp
    ${SRC_DIR}/proxy/bert_predictor.cpp
    ${SRC_DIR}/proxy/bert_predictor.hpp
    ${SRC_DIR}/proxy/proxy_manager.cpp
    ${SRC_DIR}/proxy/proxy_manager.hpp
)

set(RESPONSE_SOURCES
    ${SRC_DIR}/response/formatter.cpp
    ${SRC_DIR}/response/formatter.hpp
    ${SRC_DIR}/response/csv_generator.cpp
    ${SRC_DIR}/response/csv_generator.hpp
)

set(API_SOURCES
    ${SRC_DIR}/api/http_request.cpp
    ${SRC_DIR}/api/http_request.hpp
    ${SRC_DIR}/api/http_response.cpp
    ${SRC_DIR}/api/http_response.hpp
    ${SRC_DIR}/api/classification_controller.cpp
    ${SRC_DIR}/api/classification_controller.hpp
)

set(SERVICE_SOURCES
    ${SRC_DIR}/service/task_coordinator.cpp
    ${SRC_DIR}/service/task_coordinator.hpp
)

set(NETWORK_SOURCES
    ${SRC_DIR}/network/http_server.cpp
    ${SRC_DIR}/network/http_server.hpp
)

set(MAIN_SOURCE ${SRC_DIR}/main.cpp)

# 训练程序入口（仅训练模块，不需要HTTP服务器等依赖）
set(CORE_TRAIN_SOURCE ${SRC_DIR}/core/main.cpp)

set(INCLUDE_DIRS
    ${SRC_DIR}
    ${SRC_DIR}/common
    ${SRC_DIR}/parser
    ${SRC_DIR}/proxy
    ${SRC_DIR}/response
    ${SRC_DIR}/api
    ${SRC_DIR}/service
    ${SRC_DIR}/network
    ${SRC_DIR}/preprocessor
    ${SRC_DIR}/core
    ${SRC_DIR}/my_svm/include
    ${HTTPLIB_INCLUDE}
    ${JSON_INCLUDE}
    ${CPPJIEBA_INCLUDE}
    ${LIMONP_INCLUDE}
    ${VENDOR_DIR}
)

add_library(miniz STATIC ${MINIZ_SOURCES})
target_include_directories(miniz PUBLIC ${VENDOR_DIR}/miniz)
target_compile_definitions(miniz PUBLIC MINIZ_STATIC)

add_executable(news-classifier 
    ${MAIN_SOURCE}
    ${COMMON_SOURCES}
    ${PREPROCESSOR_SOURCES}
    ${CORE_SOURCES}
    ${MY_SVM_SOURCES}
    ${PARSER_SOURCES}
    ${PROXY_SOURCES}
    ${RESPONSE_SOURCES}
    ${API_SOURCES}
    ${SERVICE_SOURCES}
    ${NETWORK_SOURCES}
)

# 训练程序（仅包含训练模块）
add_executable(core-trainer 
    ${CORE_TRAIN_SOURCE}
    ${PREPROCESSOR_SOURCES}
    ${CORE_SOURCES}
)

target_include_directories(news-classifier PRIVATE ${INCLUDE_DIRS})
target_link_libraries(news-classifier PRIVATE miniz)

if(MSVC)
    target_compile_options(news-classifier PRIVATE /EHsc)
endif()

# 训练程序配置
target_include_directories(core-trainer PRIVATE 
    ${SRC_DIR}
    ${SRC_DIR}/core
    ${SRC_DIR}/preprocessor
    ${HTTPLIB_INCLUDE}
    ${JSON_INCLUDE}
    ${CPPJIEBA_INCLUDE}
    ${LIMONP_INCLUDE}
    ${VENDOR_DIR}
)

if(MSVC)
    target_compile_options(core-trainer PRIVATE /EHsc)
endif()

message(STATUS "========================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Source dir: ${SRC_DIR}")
message(STATUS "Executable: news-classifier")
message(STATUS "========================================")
