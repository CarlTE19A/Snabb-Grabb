// create a class inheriting from IDropTarget

class DropManager : public IDropTarget
{
public:
	//--- implement the IUnknown parts
	// you could do this the proper way with InterlockedIncrement etc,
	// but I've left out stuff that's not exactly necessary for brevity
	ULONG AddRef() { return 1; }
	ULONG Release() { return 0; }

	// we handle drop targets, let others know
	HRESULT QueryInterface(REFIID riid, void** ppvObject);

	//--- implement the IDropTarget parts

	// occurs when we drag files into our applications view
	HRESULT DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// occurs when we drag files out from our applications view
	HRESULT DragLeave();

	// occurs when we drag the mouse over our applications view whilst carrying files (post Enter, pre Leave)
	HRESULT DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);

	// occurs when we release the mouse button to finish the drag-drop operation
	HRESULT Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	~DropManager()
	{
		ClearDrop();
	}

		std::string* myPath = nullptr;
	private:
		char* myExtension = nullptr;
		const bool BeginDrop();
		const void ClearDrop();
};