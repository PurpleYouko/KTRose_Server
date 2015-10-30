/*
    Rose Online Server Emulator
    Copyright (C) 2006,2007 OSRose Team http://www.dev-osrose.com

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

    depeloped with Main erose/hrose source server + some change from the original eich source
*/

// Props to ExJam for this code :D

class CStrStb
{
public:
	CStrStb():_Data(0){}
	CStrStb(const char* filename) : _Data(0){ LoadSTB(filename); }
	~CStrStb(){CleanUp();}

	bool LoadSTB(const char* path)
    {
		CleanUp();

		CRoseFile* fh = new CRoseFile((CFILE_CHAR*) path, FM_READ | FM_BINARY);
		if(!fh->IsOpen()){
			delete fh;
			return false;
		} // if is open

		fh->Seek(4, SEEK_CUR);
		dword offset = fh->Get<dword>();

		_Rows = fh->Get<dword>();
		_Columns = fh->Get<dword>();

		_Rows--;
		_Columns--;

		fh->SetAbsPos(offset);
		_Data = new char**[_Rows];
		for(dword i = 0; i < _Rows; i++)
			_Data[i] = new char*[_Columns];

		for(dword i = 0; i < _Rows; i++){
			for(dword j = 0; j < _Columns; j++){
				dword len = fh->Get<word>();
				if(len == 0){
					_Data[i][j] = NULL;
					continue;
				} // if len
				_Data[i][j] = new char[len+1];
				fh->Read(_Data[i][j], len, 1);
				_Data[i][j][len] = 0;
			} // for j
		} // for i

		fh->Close();
		delete fh;
		return true;
    } // bool

	char* Data(dword Row, dword Column){
		if(Row > _Rows) return NULL;
		if(Column > _Columns) return NULL;

		return _Data[Row][Column];
	}

	dword Rows(){ return _Rows; }
	dword Columns(){ return _Columns; }

private:
	void CleanUp(){
		if(_Data != NULL){
			for(dword i = 0; i < _Rows; i++){
				for(dword j = 0; j < _Columns; j++){
					delete [] _Data[i][j];
				}
				delete [] _Data[i];
			}
			delete [] _Data;
		}
		_Rows = 0;
		_Columns = 0;
	}

private:
	dword _Rows;
	dword _Columns;

	char*** _Data;
};
