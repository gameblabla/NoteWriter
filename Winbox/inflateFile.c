#include <os.h>
#include <zlib.h>
#include "EasyFiles.h"






/* Decompress from file source to file dest until stream ends or EOF.
   inf() returns Z_OK on success, Z_MEM_ERROR if memory could not be
   allocated for processing, Z_DATA_ERROR if the deflate data is
   invalid or incomplete, Z_VERSION_ERROR if the version of zlib.h and
   the version of the library linked do not match, or Z_ERRNO if there
   is an error reading or writing the files.
	
	Le fichier source doit pouvoir être lu, le fichier dest écrit !!!*/
int inflateFile(FILE *source, FILE *dest)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2_(&strm, Z_WINDOW_BITS, zlibVersion(), (uint32_t) sizeof(z_stream));
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);  // retourne le nombre d'éléments correctement lus
        if (ferror(source)) {  // si une erreur dans la lecture a eu lieu
            (void)inflateEnd(&strm);
            return -2664;
        }
        if (strm.avail_in == 0)  // si on est arrivé au bout du fichier
            break;
        strm.next_in = in;


        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
				if (ret == Z_STREAM_ERROR) return ret;
				
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return -422;
            }
        } while (strm.avail_out == 0);


        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);


    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : -1333;
}






int inflateFilePart(FILE *source, FILE *dest, int bytesToRead)
{
    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2_(&strm, Z_WINDOW_BITS, zlibVersion(), (uint32_t) sizeof(z_stream));
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, min(bytesToRead, CHUNK), source);  // retourne le nombre d'éléments correctement lus
		  bytesToRead -= CHUNK;
        if (ferror(source)) {  // si une erreur dans la lecture a eu lieu
            (void)inflateEnd(&strm);
            return -2664;
        }
        if (strm.avail_in == 0)  // il y a eu une erreur de lecture !!!
            break;
        strm.next_in = in;


        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
				if (ret == Z_STREAM_ERROR) return ret;
				
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return -422;
            }
        } while (strm.avail_out == 0);


        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);


    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : -1333;
}


