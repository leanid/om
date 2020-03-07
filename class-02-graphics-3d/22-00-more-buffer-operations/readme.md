in this tutorial we should know how to use:

glBufferData - allocate memory (also can copy data)
glBufferSubData - copy data to buffer (buffer should be allocated already)
glMapBufferRange - return pointer to internal OpenGL memory of the buffer
glUnmapBuffer - return GLBool, if GL_FALSE you should reinitialize memory again