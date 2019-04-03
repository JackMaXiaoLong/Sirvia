/**
 * Author: Eric JIn
 * Date: 2019-04-02
 */
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include "../utils/stringUtil.h"

/**
 * 使用正则表达式匹配字符串
 * @param pattern
 * @param buffer
 * @return
 */
char *matchRegxOne(char *pattern, char *buffer) {
    int status = 0;
    const size_t nmatch = 1;
    regmatch_t pmatch[nmatch];
    regex_t reg;
    char *result = NULL;

    // 编译正则表达式
    if (regcomp(&reg, pattern, REG_EXTENDED) < 0) {
        printf("compile failed!\n");
        return NULL;
    }
    // 匹配正则表达式
    status = regexec(&reg, buffer, nmatch, pmatch, 0);

    if (status == 0) {
        const size_t len = (size_t) pmatch[0].rm_eo - pmatch[0].rm_so;
        result = (char *) malloc(sizeof(char) * len);
        memcpy(result, buffer + pmatch[0].rm_so, len);
    } else {
        printf("no match!\n");
        return NULL;
    }
    regfree(&reg);   // 释放正则表达式
    return result;
}

/**
 * 替换模板字符串中的内容
 * @param pattern
 * @param content
 * @param request
 * @param callback
 * @return
 */
char *replaceStr(char *pattern, char *content, cJSON *request, char *(*callback)(char *, cJSON *)) {
    int status = 0;
    const size_t matchNum = 1024;
    regmatch_t matchArr[matchNum];
    regex_t reg;

    // 编译正则表达式
    if (regcomp(&reg, pattern, REG_EXTENDED) < 0) {
        printf("compile failed!\n");
        return NULL;
    }
    // 匹配正则表达式
    status = regexec(&reg, content, matchNum, matchArr, 0);

    if (status == 0) {
        for (int i = 0; i < matchNum && matchArr[i].rm_so != -1; i++) {
            // 获取匹配到的字符串
            const size_t len = (size_t) matchArr[i].rm_eo - matchArr[i].rm_so;
            char *result = (char *) malloc(sizeof(char) * len);
            memcpy(result, content + matchArr[i].rm_so, len);
            // 对数据进行处理
            char *replace = callback(result, request);
            // 替换template里面的html文件中指定的字符串替换
            content = replaceStrFromToEnd(matchArr[i].rm_so - 1, matchArr[i].rm_eo + 1, content, replace);
        }
    } else {
        printf("no match!\n");
        return NULL;
    }
    regfree(&reg);   // 释放正则表达式
    return content;
}
