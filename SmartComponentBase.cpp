
#include "SmartComponentBase.h"

void CSmartComponentBase::packToVariant(double value, tVariant* var) {
	TV_VT(var) = VTYPE_R8;
	TV_R8(var) = value;
}

void CSmartComponentBase::packToVariant(int32_t value, tVariant* var) {
	TV_VT(var) = VTYPE_I4;
	TV_I4(var) = value;
}

void CSmartComponentBase::packToVariant(bool value, tVariant* var) {
	TV_VT(var) = VTYPE_BOOL;
	TV_BOOL(var) = value;
}

void CSmartComponentBase::packToVariant(const CBlob& blob, tVariant* var){
	packToVariant(blob.getData(), var);
	TV_VT(var) = VTYPE_BLOB;
}

void CSmartComponentBase::packToVariant(const wstring& str, tVariant* var) {
	wchar_t* ptr;
	auto size = (str.size() + 1) * sizeof(wchar_t);

	if (!mMemoryManager->AllocMemory((void**)&ptr, size)) {
		throw wstring(L"Allocation error");
	}

	memcpy(ptr, str.c_str(), size);

	TV_VT(var) = VTYPE_PWSTR;
	TV_WSTR(var) = ptr;
	var->wstrLen = str.size();
}

void CSmartComponentBase::packToVariant(const string& str, tVariant* var) {
	char* ptr;
	auto size = (str.size() + 1) * sizeof(char);

	if (!mMemoryManager->AllocMemory((void**)&ptr, size)) {
		throw wstring(L"Allocation error");
	}

	memcpy(ptr, str.c_str(), size);

	TV_VT(var) = VTYPE_PSTR;
	TV_STR(var) = ptr;
	var->strLen = str.size();
}

void CSmartComponentBase::getErrorDescription() {
	returnValue(mLastErrorDescription);
}
//---------------------------------------------------------------------------//
CSmartComponentBase::CSmartComponentBase()
	:
mMemoryManager(NULL),
	mConnect(NULL),
	mReturnValueHandler(NULL),
	mParametersHandler(NULL),
	mParametersHandlerSize(0)
{
	addMethod(L"ErrorDescription", L"��������������", 0, bind(&CSmartComponentBase::getErrorDescription, this));
}
//---------------------------------------------------------------------------//
CSmartComponentBase::~CSmartComponentBase() {

}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::Init(void* pConnection) { 
	mConnect = (IAddInDefBase*)pConnection;
	return mConnect != NULL;
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::GetInfo() { 
	return 2000; 
}
//---------------------------------------------------------------------------//
void CSmartComponentBase::Done() {

}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::RegisterExtensionAs(WCHAR_T** wsExtensionName) { 
	if (!mMemoryManager) return false;
	if (!mMemoryManager->AllocMemory((void**)wsExtensionName, (mComponentName.size() + 1) * sizeof(WCHAR_T))) return false;

	lstrcpyW(*wsExtensionName, mComponentName.c_str());

	return true; 
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::GetNProps() { 
	return mProperties.size();
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::FindProp(const WCHAR_T* wsPropName) { 
	for (unsigned long i = 0; i < mProperties.size(); i++) {
		if (_wcsicmp(mProperties[i].englishName.c_str(), wsPropName) == 0 ||
			_wcsicmp(mProperties[i].localName.c_str(), wsPropName) == 0) return i;
	}

	return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CSmartComponentBase::GetPropName(long lPropNum, long lPropAlias) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return NULL;

	wstring* name;

	if (lPropAlias == 0)
		name = &mProperties[lPropNum].englishName;
	else
		name = &mProperties[lPropNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::GetPropVal(const long lPropNum, tVariant* pvarPropVal) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	auto property = mProperties[lPropNum];

	return false;
	//return ((Derived*)this->*property.getter)(pvarPropVal);
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::SetPropVal(const long lPropNum, tVariant *varPropVal) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	auto property = mProperties[lPropNum];

	return false;
	//return ((Derived*)this->*property.setter)(varPropVal);
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::IsPropReadable(const long lPropNum) { 
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	return mProperties[lPropNum].modes & PROP_READABLE;
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::IsPropWritable(const long lPropNum) {
	if ((unsigned long)lPropNum >= mProperties.size()) return false;

	return (mProperties[lPropNum].modes & PROP_WRITEABLE) != 0;
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::GetNMethods() { 
	return mMethods.size();
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::FindMethod(const WCHAR_T* wsMethodName) { 
	for (unsigned long i = 0; i < mMethods.size(); i++) {
		if (_wcsicmp(mMethods[i].englishName.c_str(), wsMethodName) == 0 ||
			_wcsicmp(mMethods[i].localName.c_str(), wsMethodName) == 0) return i;
	}

	return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CSmartComponentBase::GetMethodName(const long lMethodNum, const long lMethodAlias) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return NULL;

	wstring* name;

	if (lMethodAlias == 0)
		name = &mMethods[lMethodNum].englishName;
	else
		name = &mMethods[lMethodNum].localName;

	if (!mMemoryManager) return NULL;

	wchar_t* result;
	if (!mMemoryManager->AllocMemory((void**)&result, (name->size() + 1) * sizeof(WCHAR_T))) return NULL;

	lstrcpyW(result, name->c_str());

	return result;
}
//---------------------------------------------------------------------------//
long CSmartComponentBase::GetNParams(const long lMethodNum) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return 0;

	return mMethods[lMethodNum].parametersCount;
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant *pvarParamDefValue) { 
	TV_VT(pvarParamDefValue)= VTYPE_EMPTY;
	return false;
} 
//---------------------------------------------------------------------------//
bool CSmartComponentBase::HasRetVal(const long lMethodNum) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return false;

	return true;
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray) { 
	return false;
}
//---------------------------------------------------------------------------//
wstring CSmartComponentBase::varToString(tVariant* Variant) {
	if (TV_VT(Variant) == VTYPE_PWSTR) {
		return wstring(TV_WSTR(Variant));
	} else if (TV_VT(Variant) == VTYPE_PSTR) {
		//return wstring(Variant->pstrVal, Variant->pstrVal + Variant->strLen);
		string temp = TV_STR(Variant);
		return wstring(temp.begin(), temp.end());
	}

	return L"";
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray) { 
	if ((unsigned long)lMethodNum >= mMethods.size()) return false;
	if (lSizeArray != mMethods[lMethodNum].parametersCount) return false;

	mReturnValueHandler = pvarRetValue;
	mParametersHandler = paParams;
	mParametersHandlerSize = lSizeArray;

	bool error = true;

	try {

		mMethods[lMethodNum].method();
		error = false;

	} catch (OKReturn) {
		error = false;
	} catch (wstring errorDescription) {
		mLastErrorDescription = errorDescription;
	} catch (...) {
		mLastErrorDescription = L"<unknown error>";
	}

	if (!error) mLastErrorDescription.clear();

	mReturnValueHandler = NULL;
	mParametersHandler = NULL;
	mParametersHandlerSize = 0;

	return !error;
}
//---------------------------------------------------------------------------//
void CSmartComponentBase::SetLocale(const WCHAR_T* loc) {
	_wsetlocale(LC_ALL, loc);
}
//---------------------------------------------------------------------------//
bool CSmartComponentBase::setMemManager(void* mem) {
	mMemoryManager = (IMemoryManager*)mem;
	return mMemoryManager != NULL;
}
//---------------------------------------------------------------------------//
//void CSmartComponentBase::addProperty(wstring englishName, wstring localName, int modes, bool (Derived::*getter)(tVariant*),
//									  bool (Derived::*setter)(tVariant*)) {
//	Property prop = { englishName, localName, modes, getter, setter };
//	mProperties.push_back(prop);
//}
//---------------------------------------------------------------------------//
void CSmartComponentBase::addMethod(wstring englishName, wstring localName, long parametersCount, componentMethod method) {
	Method meth = { englishName, localName, parametersCount, method };
	mMethods.push_back(meth);
}

