#pragma once

/*이 enum으로 현재 캐릭터가 어떤 상태인지 나타낼 수 있다. 
그리고 enum 옆에 class를 붙이는 것이 이후 사용할 때 ::를 붙여 사용하므로 안전해진다.
또한 다른 ::를 사용하므로 다른 enum과 혼동을 하지 않고 사용할 수 있다는 장점 또한 있다.
또한 CharacterState를 다른 h파일로 만듦으로써 축소시켜 include의 부담을 줄여준다.*/

UENUM(BlueprintType)		//BlueprintType을 적으면 enum을 BP에서 사용이 가능하다.
enum class ECharacterState : uint8
{
	ESC_Unequipped UMETA(DisplayName = "Unequipped"),		//여기 있는 Unequipped는 int값으로 0과 동일한 값을 가진다. 그럼 밑에는 1 그 다음은 2가 된다.
	ESC_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ESC_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};