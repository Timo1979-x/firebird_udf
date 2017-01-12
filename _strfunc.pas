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
// Возвращает 1 если во входной строке содержатся только цифровые символы
// Возвращает 0 если во входной строке присутствуют нецифровые символы
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
// Возвращает 1 если во входной строке содержатся только цифровые символы
// Возвращает 0 если во входной строке присутствуют нецифровые символы
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
// преобразует входную строку в число с учётом кривых рук диагностов,
// т.е. заменяет букву O на цифру 0 и т.д.
// на выходе либо число (номер), либо -1, если на входе явно не число
var s: String;
begin
	s := input;
	result := uCommonLib.ConvertSPNumber(s);
end;

function ComposeGNUMBER(gnumber: PChar; var gnumberdigits: Integer): PChar; cdecl; export;
// Функция объединяет части госномера - буквы и цифры
// например на входе - gnumber=@ВМ-7 и gnumberdigits=2718,
// тогда на выходе - 2718ВМ-7
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
  "Нормализует" госномера, в частности все англицкие буквы переводятся в русские,
  приводятся к верхнему регистру, удаляются ненужные пробелы и дефисы,
  добавляются дефисы в нужных местах...
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
	translit['A'] := 'А';
	translit['B'] := 'В';
	translit['C'] := 'С';
	translit['E'] := 'Е';
	translit['H'] := 'Н';
	translit['K'] := 'К';
	translit['M'] := 'М';
	translit['O'] := 'О';
	translit['P'] := 'Р';
	translit['T'] := 'Т';
	translit['X'] := 'Х';
	translit['Y'] := 'У';
  translit['I'] := 'І'; // Латинское I в беларусское I

  try
    re := TRegExpr.Create;
    regNum := input;

    //удалим серии пробельных символов:
    re.Expression := '\s{2,}';
    while re.Exec(regNum) do begin
      regNum := re.Replace(regNum, ' ', true)
    end;

    // удалим пробелы вокруг дефиса:
    re.Expression := '\s*-\s*';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '-', true);
    end;

    regNum := AnsiUpperCase(regNum);
    for i := 1 to length(regNum) do
      regNum[i] := translit[regNum[i]];

    // удалим дефис в номерах вида 45-12:
    re.Expression := '(\d\d)-(\d\d)';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1$2', true)
    end;

    // добавим дефис в номер вида '1231AA5': 1231AA5 -> 1231AA-5
    re.Expression := '([A-ZА-ЯІ])\s*(\d[^\d]*)$';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1-$2', true)
    end;

    // удалим пробел между цифрами и буквами: "4452 TT-1" -> "4452TT-1"
    re.Expression := '(@|\d{4,})\s+([A-ZА-ЯІ])';
    if re.Exec(regNum) then begin
      regNum := re.Replace(regNum, '$1$2', true)
    end;

    // удалим пробел между цифрами и буквами: "ПА 4452" -> "ПА4452"
    re.Expression := '([A-ZА-ЯІ])\s+(@|\d{4,})';
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
Корректирует строки, русские по смыслу, но введенные при английской раскладке
например @FR-2 -> @АК-2 или @F!-2 -> @АI-2

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
  corr['Q'] := 'Й';
  corr['W'] := 'Ц';
  corr['E'] := 'У';
  corr['R'] := 'К';
  corr['T'] := 'Е';
  corr['Y'] := 'Н';
  corr['U'] := 'Г';
  corr['I'] := 'Ш';
  corr['O'] := 'Щ';
  corr['P'] := 'З';
  corr['['] := 'Х';
  corr[']'] := 'Ъ';
  corr['A'] := 'Ф';
  corr['S'] := 'Ы';
  corr['D'] := 'В';
  corr['F'] := 'А';
  corr['G'] := 'П';
  corr['H'] := 'Р';
  corr['J'] := 'О';
  corr['K'] := 'Л';
  corr['L'] := 'Д';
  corr[';'] := 'Ж';
  corr[''''] := 'Э';
  corr['Z'] := 'Я';
  corr['X'] := 'Ч';
  corr['C'] := 'С';
  corr['V'] := 'М';
  corr['B'] := 'И';
  corr['N'] := 'Т';
  corr['M'] := 'Ь';
  corr[','] := 'Б';
  corr['.'] := 'Ю';
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
// Функция объединяет части госномера - буквы и цифры
// например на входе - gnumber=@ВМ-7 и gnumberdigits=2718,
// тогда на выходе - 2718ВМ-7
var tmpstr:String;
begin
	tmpstr := GnumberUtils.ExpandGNumberToString('MAPS.GNUMBER', gnumber);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

function LeadingZeroes(var value, len: Integer): PChar; cdecl; export;
// Функция форматирует входное число (value) так, чтобы впереди шли нули
// нужная длина строки (полезные цифры + нули) указывается в параметре len)
var tmpstr: String;
begin
	tmpstr := Format('%.' + IntToStr(len) + 'd', [value]);
	result := malloc(length(tmpstr) + 1);
	if result <> nil then
		StrPCopy(result, tmpstr);
end;

function ReverseString(value: PChar): PChar; cdecl; export;
// Функция возвращает "перевернутую" строку value
// например на входе - gnumber=123ab,
// тогда на выходе - ba321
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
// функция возвращает входную строку, приведенную к верхнему регистру и
// с русскими символами, замененными на похожие по начертанию
// латинские символы
// пример: 123ЙУк -> 123ЙYk (во второй строке k латинское)
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
// преобразует серию диагностической карты:
//    1. Удаляет все пробельные символы
//    2. приводит к верхнему регистру
//    3. преобразует латинские буквы к русским
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
	translit[ord('A')] := 'А';
	translit[ord('B')] := 'В';
	translit[ord('C')] := 'С';
	translit[ord('E')] := 'Е';
	translit[ord('H')] := 'Н';
	translit[ord('K')] := 'К';
	translit[ord('M')] := 'М';
	translit[ord('O')] := 'О';
	translit[ord('P')] := 'Р';
	translit[ord('T')] := 'Т';
	translit[ord('X')] := 'Х';
	translit[ord('Y')] := 'У';


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
