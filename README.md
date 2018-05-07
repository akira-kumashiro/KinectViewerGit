# KinectViewerGit
kinectの性能が試験できるはず  
*クローンの仕方は[ここ](https://github.com/akira-kumashiro/openCVtest/tree/master)に書いたよ！！！*  

## インストール方法
### 1. Visual C++ および Visual C++ の再頒布可能パッケージをインストールする
URLは[ここ](https://support.microsoft.com/ja-jp/help/3179560/update-for-visual-c-2013-and-visual-c-redistributable-package)  
### 2. OpenCV2.4.11をインストールする  
URLは[ここ](https://sourceforge.net/projects/opencvlibrary/files/opencv-win/2.4.11/opencv-2.4.11.exe/download)  
インストール先は`C:\opencv\opencv-2.4.11`(ここに`builds`と`sources`に作られれば正解)  
開発するときは新しいバージョンでしたほうがいいと思うよ  
### 3. 環境変数のPATHに追加
`C:\opencv\opencv-2.4.11\build\x64\vc12\bin`を追加  
### 4. KinectSDKをインストールする  
URLは[ここ](https://www.microsoft.com/en-us/download/details.aspx?id=44561)  
### 5. プロジェクトのプロパティの設定  
プロジェクトを右クリック→プロパティを出す  
構成プロパティのC++→全般→追加のインクルード ディレクトリに`C:\opencv\opencv-2.4.11\build\include;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\inc;%(AdditionalIncludeDirectories)`の内容で上書き  
構成プロパティのリンカー→全般→追加のライブラリ ディレクトリに`C:\opencv\opencv-2.4.11\build\x64\vc12\lib;C:\Program Files\Microsoft SDKs\Kinect\v2.0_1409\Lib\x64;%(AdditionalLibraryDirectories)`の内容で上書き  
構成プロパティのリンカー→入力→追加の依存ファイルに`kinect20.lib;%(AdditionalDependencies)`の内容で上書き  
  
## Updateブランチについて  
もともとあったコードがゴミコード過ぎたので修正します  
~~動作確認したらmasterに反映します~~ マージしました  
  
## その他
困ったら連絡して
