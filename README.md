Paper Plane xUI  FS Module

PPx FS Module �́A�ȉ��̊֐��E�R�}���h������܂��B

�R�}���h *makedirlist([["�p�X"] [, ["���C���h�J�[�h"] [, �I�v�V����]]])
�֐�     %*makedirlist([["�p�X"] [, ["���C���h�J�[�h"] [, �I�v�V����]]])

�w��p�X�̃f�B���N�g���ꗗ���e�L�X�g�Ŏ擾���܂��B�֐��ł�
�擾�����e�L�X�g��Ԃ��܂��B
	
��1) *makedirlist "%'temp'", , utf8, nosubdir, "%'temp'%\result.txt"
��2) *logwindow "%*makedirlist("%'temp'", *.tmp)"

�I�v�V�����́A�ȉ��̎w����u,�v��؂�Ŏw��ł��܂��B

	"�t�@�C����" �ꗗ��ۑ�����t�@�C���̖��O�ł��B

	subdir, nosubdir
		subdir ���w�肷��ƃT�u�f�B���N�g������
		�f�B���N�g�����񋓂��܂��Bnosubdir �͗񋓂��܂���B
		�����l�� subdir �ł��B

	utf8, utf8bom, system, unicode, unicodebom
		�t�@�C���ɕۑ�����Ƃ��̕����R�[�h�ł��B
		�����l�� unicodebom �ł��B
