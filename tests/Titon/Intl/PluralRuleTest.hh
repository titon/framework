<?hh
namespace Titon\Intl;

use Titon\Test\TestCase;

class PluralRuleTest extends TestCase {

    public function testCalculateRule2(): void {
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_2, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_2, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_2, 5));
    }

    public function testCalculateRule3(): void {
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_3, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_3, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_3, 5));
    }

    public function testCalculateRule4(): void {
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_4, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_4, 1));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_4, 101));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_4, 111));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_4, 112));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_4, 124));
    }

    public function testCalculateRule5(): void {
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_5, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_5, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_5, 4));
    }

    public function testCalculateRule6(): void {
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_6, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_6, 2));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_6, 5));
    }

    public function testCalculateRule7(): void {
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_7, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_7, 1));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_7, 101));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_7, 111));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_7, 112));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_7, 124));
    }

    public function testCalculateRule8(): void {
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_8, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_8, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_8, 2));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_8, 101));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_8, 202));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_8, 303));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_8, 404));
    }

    public function testCalculateRule9(): void {
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_9, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_9, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_9, 2));
    }

    public function testCalculateRule10(): void {
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_10, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_10, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_10, 2));
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_10, 101));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_10, 212));
    }

    public function testCalculateRule11(): void {
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_11, 0));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_11, 1));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_11, 2));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_11, 101));
    }

    public function testCalculateRule12(): void {
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_12, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_12, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_12, 2));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_12, 101));
    }

    public function testCalculateRule13(): void {
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_13, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_13, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_13, 2));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_13, 8));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_13, 11));
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_13, 100));
    }

    public function testCalculateRule14(): void {
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_14, 0));
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_14, 1));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_14, 2));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_14, 10));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_14, 100));
    }

    public function testCalculateRule15(): void {
        $this->assertEquals(0, PluralRule::calculate(PluralRule::RULE_15, 0));
        $this->assertEquals(1, PluralRule::calculate(PluralRule::RULE_15, 1));
        $this->assertEquals(2, PluralRule::calculate(PluralRule::RULE_15, 2));
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_15, 10));
        $this->assertEquals(4, PluralRule::calculate(PluralRule::RULE_15, 111));
        $this->assertEquals(3, PluralRule::calculate(PluralRule::RULE_15, 204));
    }

}
