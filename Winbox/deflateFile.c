#include <os.h>
#include <zlib.h>
#include "EasyFiles.h"





/* Compress from file source to file dest until EOF on source.
   def() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_STREAM_ERROR if an invalid compression
   level is supplied, Z_VERSION_ERROR if the version of zlib.h and the
   version of the library linked do not match, or Z_ERRNO if there is
   an error reading or writing the files. */
int deflateFile(FILE *source, FILE *dest, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	/* allocate deflate state */
	strm.next_in = Z_NULL;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit2_(&strm, level, Z_DEFLATED, Z_WINDOW_BITS, Z_MEMORY_LEVEL, Z_STRATEGY, zlibVersion(), (uint32_t) sizeof(z_stream));
	if (ret != Z_OK)
	  return ret;



	/* compress until end of file */
	do {
		strm.avail_in = fread(in, 1, CHUNK, source);
		if (ferror(source)) {
			(void)deflateEnd(&strm);
			// return Z_ERRNO;
			return -57;
		}
		flush = feof(source) ? Z_FINISH : Z_NO_FLUSH;
		strm.next_in = in;
		
		
		/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			
			
			ret = deflate(&strm, flush);    /* no bad return value */
			if (ret == Z_STREAM_ERROR) return ret;
			
			
			
			have = CHUNK - strm.avail_out;
			fwrite(out, 1, have, dest);  // il n'y a aucune raison que cela créé une erreur !!!
			
			
		} while (strm.avail_out == 0);
		if (strm.avail_in != 0) return -23;
		
		
		
		/* done when last data in file processed */
	} while (flush != Z_FINISH);
	if (ret != Z_STREAM_END) return ret;
		
	
	
	/* clean up and return */
	deflateEnd(&strm);
	return Z_OK;
}


