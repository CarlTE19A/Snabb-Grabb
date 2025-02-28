class Stencil
{
public:
	enum class eMode
	{
		off,
		Write,
		Mask
	};
	Stencil(eMode aMode);

	void Bind();

private:
	ComPtr<ID3D11DepthStencilState> myStencilBuffer;
};