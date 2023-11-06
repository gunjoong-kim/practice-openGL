#include "Texture.hpp"

TextureUPtr Texture::CreateFromImage(const Image* image) {
    auto texture = TextureUPtr(new Texture());
    texture->CreateTexture();
    texture->SetTextureFromImage(image);
    return std::move(texture);
}

Texture::~Texture() {
    if (m_texture) {
        glDeleteTextures(1, &m_texture);
    }
}

void Texture::Bind() const {
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::SetFilter(uint32_t minFilter, uint32_t magFilter) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::SetWrap(uint32_t sWrap, uint32_t tWrap) const {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sWrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tWrap);
}
    
void Texture::CreateTexture() {
    glGenTextures(1, &m_texture);
    // bind and set default filter and wrap option
    Bind();
    SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    SetWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
}

void Texture::SetTextureFromImage(const Image* image) {
    GLenum format = GL_RGBA;
    switch (image->GetChannelCount()) {
        default: break;
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        image->GetWidth(), image->GetHeight(), 0,
        format, GL_UNSIGNED_BYTE,
        image->GetData());
	
	glGenerateMipmap(GL_TEXTURE_2D);
}

//glGenTextures(1, &(this->m_texture));
	//glBindTexture(GL_TEXTURE_2D, this->m_texture);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // texture 너무 크면 어떻게 할까
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 반대
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 가로 0 보다 작거나 1보다 클때
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // 세로 0 보다 작거나 1보다 클때

	//// target : 2D texture <- m_texture에 binding 되어 있음
	//// 앞 : GPU texture data
	//// 뒤 : CPU texture data
	//// CPU -> GPU 복사 해야 하므로 여러 description 명세
	//// 0 <- level 값 <- min map
	//// GL_RGB -> channel
	//// 0 <- boarder size
	//// image에서의 format
	//// 하나의 채널을 표현하는 데이터 타입
	//// 실제 데이터 넘겨주기
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
	//	image->GetWidth(), image->GetHeight(), 0,
	//	GL_RGB, GL_UNSIGNED_BYTE, image->GetData());