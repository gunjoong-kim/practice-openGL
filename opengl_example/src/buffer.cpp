#include "Buffer.hpp"

BufferUPtr Buffer::CreateWithData(
    uint32_t bufferType,
	uint32_t usage,
    const void* data,
	size_t dataSize
) {
    auto buffer = BufferUPtr(new Buffer());

    if (!buffer->Init(bufferType, usage, data, dataSize))
        return nullptr;
    return std::move(buffer);
}

Buffer::~Buffer()
{
    if (this->m_buffer) {
        glDeleteBuffers(1, &(this->m_buffer));
    }
}

void Buffer::Bind() const
{
    glBindBuffer(this->m_bufferType, this->m_buffer);
}

bool Buffer::Init(
    uint32_t bufferType,
    uint32_t usage,
    const void* data,
    size_t dataSize
) {
    this->m_bufferType = bufferType;
    this->m_usage = usage;
    glGenBuffers(1, &(this->m_buffer));
    Bind();
    glBufferData(this->m_bufferType, dataSize, data, usage);
    return true;
}