using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CardComponent : MonoBehaviour
{
    public enum CardType
    {
		DoubleJump,
		Dash,
		Stomp,
		DirectionalDash,
		Grapple,
		Teleport,
		Bomb
	};

    public CardType cardType;

	//draw gizmos
	void OnDrawGizmos()
	{
		switch (cardType)
		{
			case CardType.DoubleJump:
				Gizmos.color = new Color(0.8f,0.4f,0);
				break;
			case CardType.Dash:
				Gizmos.color = new Color(0.8f,0,0.7f);
				break;
            case CardType.Stomp:
                Gizmos.color = new Color(0,0.2f,0.8f);
				break;
			case CardType.DirectionalDash:
                Gizmos.color = new Color(0.3f, 1, 0.1f);
                break;
			case CardType.Grapple:
				Gizmos.color = new Color(0.9f, 0.1f, 0.1f);
                break;
			case CardType.Teleport:
				Gizmos.color = new Color(0.3f, 0.9f, 0.9f);
				break;
			case CardType.Bomb:
				Gizmos.color = new Color(0.9f, 0.9f, 0.4f);
				break;
			default:
				break;
		}

		Gizmos.DrawWireSphere(transform.position, 0.5f);
	}
}
