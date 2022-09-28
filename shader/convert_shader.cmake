set(SHADER_NAME "${CMAKE_ARGV3}")
set(SHADER "${CMAKE_ARGV4}")
set(SHADER_HEADER "${CMAKE_ARGV5}")

file(READ "${SHADER}" CONTENT)
file(WRITE "${SHADER_HEADER}" "#pragma once\n\n\
\
const char ${SHADER_NAME}[] = R\"(\n\
${CONTENT}\
)\";\n\
")
