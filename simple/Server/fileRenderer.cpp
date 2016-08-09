#include "muskat.h"

void FileRenderer::getFrame(FrameInfo& info, FrameBuffer& fb) {
	//fb = m_pScene->m_fbs[info.id];
	fb = m_pScene->m_fbs[0];
}

void FileRenderer::setScene(Scene* pScene) {
	m_pScene = pScene;
}
