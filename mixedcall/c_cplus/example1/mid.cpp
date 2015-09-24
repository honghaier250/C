#include <iostream>

void world();

#ifdef __cplusplus
extern "C"
{
#endif

void m_world()
{
    world();
}

#ifdef __cplusplus
}
#endif
