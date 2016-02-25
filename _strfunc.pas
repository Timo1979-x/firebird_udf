unit _strfunc;

{ Important note about DLL memory management: ShareMem must be the
	first unit in your library's USES clause AND your project's (select
	Project-View Source) USES clause if your DLL exports any procedures or
	functions that pass strings as parameters or function results. This
	applies to all strings passed to and from your DLL--even those that
	are nested in records and classes. ShareMem is the interface unit to
	the BORLNDMM.DLL shared memory manager, which must be deployed along
	with your DLL. To avoid using BORLNDMM.DLL, pass string information
	using PChar or ShortString parameters. }
interface
uses
  SysUtils,
  uCommonLib,
  RegExpr,
  GnumberUtils,
  uStringLib,
  UMyTypes;

function ConvertSPNumber(input: PChar): Integer; cdecl; export;
function ContainsOnlyDigits(input: PChar): Integer; cdecl; export;
function my_strcopy(inp: PChar; var start, len: Integer): PChar; cdecl; export;
function returninteger: integer; cdecl; export;
function GetGNumberDigits(GNumber: PChar): Integer; cdecl; export;
function GetGNumberLetters(GNumber: PChar): PChar; cdecl; export;
function returnnullstring: PChar; cdecl; export;
function expandGNUMBER(gnumber: PChar): PChar; cdecl; export;
function CorrectLayoutLat2Rus(inp: PChar): PChar; cdecl; export;
function NormalizeDKSeries(value: PChar): PChar; cdecl; export;
function LeadingZeroes(var value, len: Integer): PChar; cdecl; export;
function normalizeRegNumber(input: Pchar): Pchar; cdecl; export;
function ComposeGNUMBER(gnumber: PChar; var gnumberdigits: Integer): PChar; cdecl; export;
function StrToInt(input: PChar): Integer; cdecl; export;
function ReverseString(value: PChar): PChar; cdecl; export;
function LatinizeUppercase(value: PChar): PChar; cdecl; export;

implementation

function malloc(Size: Integer): Pointer; cdecl; external 'msvcrt.dll';

{
DECLARE EXTERNAL FUNCTION STRCOPY
		CSTRING(255),
		INTEGER,
		INTEGER
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'my_strcopy' MODULE_NAME 'strfunc.dll'
}
function my_strcopy(inp: PChar; var start, len: Integer): PChar; cdecl; export;
var tmpstr: String;
begin
	tmpstr := copy(inp, start + 1, len);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

{
DECLARE EXTERNAL FUNCTION RETURNINTEGER
RETURNS INTEGER BY VALUE
ENTRY_POINT 'returninteger' MODULE_NAME 'strfunc.dll'
}
function returninteger: integer; cdecl; export;
begin
{	result:=malloc(sizeof(Integer));
	if assigned(result) then
		result^:=123321;}
	result := 321123;

end;

function GetGNumberDigits(GNumber: PChar): Integer; cdecl; export;
var gnum: Integer;
	newgnumber: String;
begin
	if _parseGNumber(gNumber, gnum, newgnumber) then
		result := gnum
	else
		result := -1;
end;

{function StringContainsOnlyDigits(inputstr: PChar): Integer; cdecl; export;
// ���������� 1 ���� �� ������� ������ ���������� ������ �������� �������
// ���������� 0 ���� �� ������� ������ ������������ ���������� �������
begin
	if uCommonLib.ContainsOnlyDigits(inputstr) then
		result := 1
	else
		result := 0;
end;
}

function GetGNumberLetters(GNumber: PChar): PChar; cdecl; export;
var gnum: Integer;
	newgnumber: String;
begin
	_parseGNumber(gNumber, gnum, newgnumber);
	result := malloc(length(newgnumber) + 1);
	if result <> nil then
		StrPCopy(result, newgnumber);
end;

function returnnullstring: PChar; cdecl; export;
begin
	result := nil;
end;


function ContainsOnlyDigits(input: PChar): Integer; cdecl; export;
// ���������� 1 ���� �� ������� ������ ���������� ������ �������� �������
// ���������� 0 ���� �� ������� ������ ������������ ���������� �������
var s: String;
begin
	s := sysutils.trim(input);
	try
		sysutils.StrToInt(s);
		result := 1;
	except
		result := 0;
  end;
end;

{function ContainsOnlyDigits(input:PChar):Integer;cdecl;export;
var i,l:Integer;
	s:string;
begin
	result:=0;
	s:=input;
	l:=length(s);
	if l=0 then
		exit;

	for I := 1 to l do
	begin
		//if (((s[i]<'0') or (s[i]>'9')) and (ord(s[i])<>0)) then
		if ((s[i]<'0') or (s[i]>'9')) then
			exit;
	end;

	result:=1;
end;}


function StrToInt(input: PChar): Integer; cdecl; export;
begin
	result := MyStrToInt(input);
end;

function ConvertSPNumber(input: PChar): Integer; cdecl; export;
// ����������� ������� ������ � ����� � ������ ������ ��� ����������,
// �.�. �������� ����� O �� ����� 0 � �.�.
// �� ������ ���� ����� (�����), ���� -1, ���� �� ����� ���� �� �����
var s: String;
begin
	s := input;
	result := uCommonLib.ConvertSPNumber(s);
end;

function ComposeGNUMBER(gnumber: PChar; var gnumberdigits: Integer): PChar; cdecl; export;
// ������� ���������� ����� ��������� - ����� � �����
// �������� �� ����� - gnumber=@��-7 � gnumberdigits=2718,
// ����� �� ������ - 2718��-7
{
DECLARE EXTERNAL FUNCTION COMPOSEGNUMBER
    CSTRING(255),
    INTEGER
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'ComposeGNUMBER' MODULE_NAME 'strfunc.dll';
}
var tmpstr: String;
begin
	tmpstr :=
		GnumberUtils.ComposeGNumber(gnumber, gnumberDigits);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

{
DECLARE EXTERNAL FUNCTION NORMALIZEREGNUMBER
		CSTRING(255)
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'NormalizeRegNumber' MODULE_NAME 'strfunc.dll'
}
function NormalizeRegNumber(input: Pchar): Pchar; cdecl; export;
{
  "�����������" ���������, � ��������� ��� ��������� ����� ����������� � �������,
  ���������� � �������� ��������, ��������� �������� ������� � ������,
  ����������� ������ � ������ ������...
}
var re: TRegExpr;
  regNum: String;
  i: Integer;
  c: Char;
  translit: array [char] of char;
begin
  re := nil;
  for c := low(translit) to high(translit) do
		translit[c] := c;
	translit['A'] := '�';
	translit['B'] := '�';
	translit['C'] := '�';
	translit['E'] := '�';
	translit['H'] := '�';
	translit['K'] := '�';
	translit['M'] := '�';
	translit['O'] := '�';
	translit['P'] := '�';
	translit['T'] := '�';
	translit['X'] := '�';
	translit['Y'] := '�';
  translit['I'] := '�'; // ��������� I � ����������� I

  try
    re := TRegExpr.Create;
    regNum := input;

    //������ ����� ���������� ��������:
    re.Expression := '\s{2,}';
    while re.Exec(regNum) do begin
      regNum := re.Replace(regNum, ' ', true)
    end;

    // ������ ������� ������ ������:
    re.Expression := '\s*-\s*';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '-', true);
    end;

    regNum := AnsiUpperCase(regNum);
    for i := 1 to length(regNum) do
      regNum[i] := translit[regNum[i]];

    // ������ ����� � ������� ���� 45-12:
    re.Expression := '(\d\d)-(\d\d)';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1$2', true)
    end;

    // ������� ����� � ����� ���� '1231AA5': 1231AA5 -> 1231AA-5
    re.Expression := '([A-Z�-߲])\s*(\d[^\d]*)$';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1-$2', true)
    end;

    // ������ ������ ����� ������� � �������: "4452 TT-1" -> "4452TT-1"
    re.Expression := '(@|\d{4,})\s+([A-Z�-߲])';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1$2', true)
    end;

    // ������ ������ ����� ������� � �������: "�� 4452" -> "��4452"
    re.Expression := '([A-Z�-߲])\s+(@|\d{4,})';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1$2', true)
    end;
    regNum := Trim(regNum);
  finally
    if assigned(re) then
      FreeAndNil(re);
  end;
  result := malloc(length(regNum) + 1);
  if result <> nil then
    StrPCopy(result, regNum);
end;

function CorrectLayoutLat2Rus(inp: PChar): PChar; cdecl; export;
{
������������ ������, ������� �� ������, �� ��������� ��� ���������� ���������
�������� @FR-2 -> @��-2 ��� @F!-2 -> @�I-2

DECLARE EXTERNAL FUNCTION CorrectLayoutLat2Rus
    CSTRING(255)
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'CorrectLayoutLat2Rus' MODULE_NAME 'strfunc.dll';
}
var i: Integer;
    inputString, outputString: String;
    corr: array[char] of char;
begin
  for i := 0 to 255 do
    corr[chr(i)] := chr(i);
  corr['!'] := 'I';
  corr['Q'] := '�';
  corr['W'] := '�';
  corr['E'] := '�';
  corr['R'] := '�';
  corr['T'] := '�';
  corr['Y'] := '�';
  corr['U'] := '�';
  corr['I'] := '�';
  corr['O'] := '�';
  corr['P'] := '�';
  corr['['] := '�';
  corr[']'] := '�';
  corr['A'] := '�';
  corr['S'] := '�';
  corr['D'] := '�';
  corr['F'] := '�';
  corr['G'] := '�';
  corr['H'] := '�';
  corr['J'] := '�';
  corr['K'] := '�';
  corr['L'] := '�';
  corr[';'] := '�';
  corr[''''] := '�';
  corr['Z'] := '�';
  corr['X'] := '�';
  corr['C'] := '�';
  corr['V'] := '�';
  corr['B'] := '�';
  corr['N'] := '�';
  corr['M'] := '�';
  corr[','] := '�';
  corr['.'] := '�';
  inputString := inp;
  inputString := AnsiUpperCase(inputString);

  outputString := '';
  for I := 1 to Length(inputString) do
    outputString := outputString + corr[inputString[i]];

  result := malloc(length(outputString) + 1);
  if result <> nil then
    StrPCopy(result, outputString);
end;

function expandGNUMBER(gnumber: PChar): PChar; cdecl; export;
// ������� ���������� ����� ��������� - ����� � �����
// �������� �� ����� - gnumber=@��-7 � gnumberdigits=2718,
// ����� �� ������ - 2718��-7
var tmpstr:String;
begin
	tmpstr := GnumberUtils.ExpandGNumberToString('MAPS.GNUMBER', gnumber);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

function LeadingZeroes(var value, len: Integer): PChar; cdecl; export;
// ������� ����������� ������� ����� (value) ���, ����� ������� ��� ����
// ������ ����� ������ (�������� ����� + ����) ����������� � ��������� len)
var tmpstr: String;
begin
	tmpstr := Format('%.' + IntToStr(len) + 'd', [value]);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

function ReverseString(value: PChar): PChar; cdecl; export;
// ������� ���������� "������������" ������ value
// �������� �� ����� - gnumber=123ab,
// ����� �� ������ - ba321
{
DECLARE EXTERNAL FUNCTION REVERSESTRING
    CSTRING(255)
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'ReverseString' MODULE_NAME 'strfunc.dll';
}
var tmpstr, rslt: String;
  len, i: Integer;
begin
  tmpstr := Trim(value);
  len := Length(tmpstr);
  SetLength(rslt, len);
  for I := len downto 1 do begin
    rslt[len - i + 1] := tmpstr[i];
  end;

	result := malloc(length(rslt) + 1);
	if result <> nil then
		StrPCopy(result, rslt);
end;

function LatinizeUppercase(value: PChar): PChar; cdecl; export;
// ������� ���������� ������� ������, ����������� � �������� �������� �
// � �������� ���������, ����������� �� ������� �� ����������
// ��������� �������
// ������: 123��� -> 123�Yk (�� ������ ������ k ���������)
{
DECLARE EXTERNAL FUNCTION LATINIZEUPPERCASE
    CSTRING(255)
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'LatinizeUppercase' MODULE_NAME 'strfunc.dll';
}
var rslt: String;
  len, i: Integer;
begin
  rslt := Latinize(AnsiUpperCase(value));

	result := malloc(length(rslt) + 1);
	if result <> nil then
		StrPCopy(result, rslt);
end;

function NormalizeDKSeries(value: PChar): PChar; cdecl; export;
// ����������� ����� ��������������� �����:
//    1. ������� ��� ���������� �������
//    2. �������� � �������� ��������
//    3. ����������� ��������� ����� � �������
{
DECLARE EXTERNAL FUNCTION NormalizeDKSeries
    CSTRING(255)
RETURNS CSTRING(255) FREE_IT
ENTRY_POINT 'NormalizeDKSeries' MODULE_NAME 'strfunc.dll';
}
var translit: array [0..255] of char;
var in_str, tmpstr, rslt: String;
  len, i: Integer;
  ch: char;
begin
  for i := low(translit) to high(translit) do
		translit[i] := chr(i);
	translit[ord('A')] := '�';
	translit[ord('B')] := '�';
	translit[ord('C')] := '�';
	translit[ord('E')] := '�';
	translit[ord('H')] := '�';
	translit[ord('K')] := '�';
	translit[ord('M')] := '�';
	translit[ord('O')] := '�';
	translit[ord('P')] := '�';
	translit[ord('T')] := '�';
	translit[ord('X')] := '�';
	translit[ord('Y')] := '�';


  in_str := AnsiUpperCase(value);
  len := Length(in_str);
  rslt := '';

  for I := 1 to len do begin
    ch := in_str[i];
    if not (ch in [' ', chr(9), chr(10), chr(13)]) then begin
      rslt := rslt + translit[ord(ch)];
    end;
  end;

	result := malloc(length(rslt) + 1);
	if result <> nil then
		StrPCopy(result, rslt);
end;


begin
end.
