#pragma once

class UISprite;
class UIButton;
class UIClock;
class UIElement;

class UIHandler
{
public:
	void Init(std::string aName = "unnamed");
	~UIHandler();
	void Update(float aDeltaTime);
	void Draw();
	
	void CreateUIElement();
	
	void CreateUIButton(const std::vector<std::string> aPaths, Vector2f aPosition, Vector2f& aSize);
	void CreateSprite(const std::string& aPath, Vector2f& aPosition, Vector2f& aSize);
	
	
	//void CreateUIClock(const std::string& aPath, const Vector2f& aPosition, Vector2f& aSize, const float aCooldownTime);
	//UIButton* GetButton(int aIndex) { return myButtons[aIndex]; }
	//UISprite* GetSprite(int aIndex) { return mySprites[aIndex]; }
	//UIClock* GetClock(int aIndex) { return myClocks[aIndex]; }
	std::vector<UIElement*> GetElements() { return myElements; }
	//void StartCooldown(int aIndex);
	std::string GetName() { return myName; }

	void ReadUILayout();
	void SaveUILayout();

	void SetVisible(bool aVisible) { myIsVisible = aVisible; }
	bool& IsVisible() { return myIsVisible; }

	void MoveElementToFront(int aID);
	void MoveElementToBack(int aID);
	void MoveElementForward(int aID);
	void MoveElementBackward(int aID);

	void SwapElements(int aID1, int aID2);

	void RemoveElement(int aID);

private:
	std::map<int, UIElement*> myIDMap;
	std::vector<UIElement*> myElements;
	//std::vector<UIButton*> myButtons;
	//std::vector<UISprite*> mySprites;
	//std::vector<UIClock*> myClocks;
	std::string myName;
	bool myIsVisible = false;
};

