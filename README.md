Paper Plane xUI  FS Module

PPx FS Module は、以下の関数・コマンドがあります。

コマンド *makedirlist([["パス"] [, ["ワイルドカード"] [, オプション]]])
関数     %*makedirlist([["パス"] [, ["ワイルドカード"] [, オプション]]])

指定パスのディレクトリ一覧をテキストで取得します。関数版は
取得したテキストを返します。
	
例1) *makedirlist "%'temp'", , utf8, nosubdir, "%'temp'%\result.txt"
例2) *logwindow "%*makedirlist("%'temp'", *.tmp)"

オプションは、以下の指定を「,」区切りで指定できます。

	"ファイル名" 一覧を保存するファイルの名前です。

	subdir, nosubdir
		subdir を指定するとサブディレクトリ内の
		ディレクトリも列挙します。nosubdir は列挙しません。
		初期値は subdir です。

	utf8, utf8bom, system, unicode, unicodebom
		ファイルに保存するときの文字コードです。
		初期値は unicodebom です。
