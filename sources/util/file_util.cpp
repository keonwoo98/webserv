//
// Created by Alvin Lee on 2022/12/25.
//
#include <fcntl.h>
#include <cstdio>

#include "udata.hpp"

void CheckStaticFileOpenError(int fd) {
	if (fd < 0) {
		if (errno == ENOENT) {
			throw HttpException(NOT_FOUND, std::strerror(errno));
		}
		if (errno == EACCES) {
			throw HttpException(FORBIDDEN, std::strerror(errno));
		}
		throw HttpException(NOT_ACCEPTABLE, std::strerror(errno));
	}
}

/**
 * Get File Size
 * - fopen: open file
 * - fseek: sets the file position indicator for the stream pointed to by stream.
 * - ftell: obtain the current value of the file position indicator for the stream pointed to by stream.
 * - rewind : sets the file position indicator for the stream pointed to by stream to the beginning of the file.
 * @param fp
 * @return if success size of the file. otherwise -1
 */
long GetFileSize(const char *file_path) {
	FILE *fp = fopen(file_path, "r");
	if (fp == NULL) {
		return -1;
	}
	if (fseek(fp, 0L, SEEK_END)) {
		fclose(fp);
		return -1;
	}
	long size = ftell(fp); // if ftell fails : -1
	fclose(fp);
	return size;
}
