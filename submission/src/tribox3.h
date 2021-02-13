// http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox_tam.pdf
// http://fileadmin.cs.lth.se/cs/personal/tomas_akenine-moller/code/tribox3.txt

#ifndef HAVE_TRIBOX3_H
#define HAVE_TRIBOX3_H

#ifdef __cplusplus
extern "C" {
#endif

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3]);

#ifdef __cplusplus
}
#endif

#endif
