<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class ValidateTest extends TestCase {

    protected $image;

    protected function setUp(): void {
        parent::setUp();

        $this->image = TEMP_DIR . '/utility/image.jpg';
    }

    public function testAlpha(): void {
        $this->assertTrue(Validate::alpha('ahjsNKHAShksdnASQfgd'));
        $this->assertTrue(Validate::alpha('asdnasdsd.dfsdfdfsdfs;', '.;'));

        $this->assertFalse(Validate::alpha('asdnasdsd.dfsdfdfsdfs;'));
        $this->assertFalse(Validate::alpha('asdjn1803201'));
        $this->assertFalse(Validate::alpha('pkmpwij*39@0'));
    }

    public function testAlphaNumeric(): void {
        $this->assertTrue(Validate::alphaNumeric('ahjsNKHAShksdnASQfgd'));
        $this->assertTrue(Validate::alphaNumeric('asdnasdsd.dfsdfdfsdfs;', '.;'));
        $this->assertTrue(Validate::alphaNumeric('asdjn1803201'));

        $this->assertFalse(Validate::alphaNumeric('asdnasdsd.dfsdfdfsdfs;'));
        $this->assertFalse(Validate::alphaNumeric('pkmpwij*39@0'));
    }

    public function testBetween(): void {
        $this->assertTrue(Validate::between('This is just the right length', 10, 30));

        $this->assertFalse(Validate::between('This is far too long because its more than 30 characters', 10, 30));
        $this->assertFalse(Validate::between('Too short', 10, 30));
    }

    public function testBoolean(): void {
        $this->assertTrue(Validate::boolean(true));
        $this->assertTrue(Validate::boolean(false));
        $this->assertTrue(Validate::boolean(0));
        $this->assertTrue(Validate::boolean(1));
        $this->assertTrue(Validate::boolean('yes'));
        $this->assertTrue(Validate::boolean('off'));

        $this->assertFalse(Validate::boolean(null));
        $this->assertFalse(Validate::boolean(''));
        $this->assertFalse(Validate::boolean(123));
        $this->assertFalse(Validate::boolean('abc'));
    }

    public function testCallback(): void {
        $this->assertTrue(Validate::callback('123', ($value) ==> is_numeric($value)));

        $this->assertFalse(Validate::callback('123abc', ($value) ==> is_numeric($value)));
    }

    public function testComparison(): void {
        $this->assertTrue(Validate::comparison(15, 10, '>'));
        $this->assertFalse(Validate::comparison(5, 10, 'gt'));

        $this->assertTrue(Validate::comparison(10, 10, '>='));
        $this->assertFalse(Validate::comparison(5, 10, 'gte'));

        $this->assertTrue(Validate::comparison(5, 10, '<'));
        $this->assertFalse(Validate::comparison(15, 10, 'lt'));

        $this->assertTrue(Validate::comparison(10, 10, '<='));
        $this->assertFalse(Validate::comparison(15, 10, 'lte'));

        $this->assertTrue(Validate::comparison(10, 10, '=='));
        $this->assertFalse(Validate::comparison(15, 10, 'eq'));

        $this->assertTrue(Validate::comparison(5, 10, '!='));
        $this->assertFalse(Validate::comparison(10, 10, 'ne'));
    }

    /**
     * @expectedException \Titon\Common\Exception\InvalidArgumentException
     */
    public function testComparisonInvalidOperator(): void {
        Validate::comparison(10, 10, '><');
    }

    public function testCreditCard(): void {
        $this->assertTrue(Validate::creditCard('4916933155767'));
        $this->assertFalse(Validate::creditCard('2346533'));

        // American express
        $this->assertTrue(Validate::creditCard('377147515754475', Validate::AMERICAN_EXPRESS));
        $this->assertTrue(Validate::creditCard('375239372816422', Validate::AMERICAN_EXPRESS));

        // Bankcard
        $this->assertTrue(Validate::creditCard('5602248780118788', Validate::BANKCARD));
        $this->assertTrue(Validate::creditCard('5610631567676765', Validate::BANKCARD));

        // Diners club 14
        $this->assertTrue(Validate::creditCard('30158334709185', Validate::DINERS_CLUB));
        $this->assertTrue(Validate::creditCard('30195413721186', Validate::DINERS_CLUB));
        $this->assertTrue(Validate::creditCard('5577265786122391', Validate::DINERS_CLUB));
        $this->assertTrue(Validate::creditCard('5534061404676989', Validate::DINERS_CLUB));

        // Discover
        $this->assertTrue(Validate::creditCard('6509735979634270', Validate::DISCOVER));
        $this->assertTrue(Validate::creditCard('6011422366775856', Validate::DISCOVER));

        // enRoute
        $this->assertTrue(Validate::creditCard('214945833739665', Validate::ENROUTE));
        $this->assertTrue(Validate::creditCard('214982692491187', Validate::ENROUTE));

        // JCB
        $this->assertTrue(Validate::creditCard('180031358949367', Validate::JCB));
        $this->assertTrue(Validate::creditCard('180033802147846', Validate::JCB));
        $this->assertTrue(Validate::creditCard('3158671691305165', Validate::JCB));
        $this->assertTrue(Validate::creditCard('3528523028771093', Validate::JCB));

        // Maestro
        $this->assertTrue(Validate::creditCard('5020412965470224', Validate::MAESTRO));
        $this->assertTrue(Validate::creditCard('5020129740944022', Validate::MAESTRO));

        // Mastercard
        $this->assertTrue(Validate::creditCard('5538725892618432', Validate::MASTERCARD));
        $this->assertTrue(Validate::creditCard('5119543573129778', Validate::MASTERCARD));

        // Solo
        $this->assertTrue(Validate::creditCard('6334768185398134', Validate::SOLO_DEBIT));
        $this->assertTrue(Validate::creditCard('633487484858610484', Validate::SOLO_DEBIT));
        $this->assertTrue(Validate::creditCard('6767838565218340113', Validate::SOLO_DEBIT));

        // Switch
        $this->assertTrue(Validate::creditCard('4936295218139423', Validate::SWITCH_DEBIT));
        $this->assertTrue(Validate::creditCard('493691609704348548', Validate::SWITCH_DEBIT));
        $this->assertTrue(Validate::creditCard('4936510653566569547', Validate::SWITCH_DEBIT));

        // Visa
        $this->assertTrue(Validate::creditCard('4916933155767', Validate::VISA));
        $this->assertTrue(Validate::creditCard('4024007159672', Validate::VISA));
        $this->assertTrue(Validate::creditCard('4481007485188614', Validate::VISA));
        $this->assertTrue(Validate::creditCard('4716533372139623', Validate::VISA));

        // Visa electron
        $this->assertTrue(Validate::creditCard('4175005028142917', Validate::VISA_ELECTRON));

        // Voyager
        $this->assertTrue(Validate::creditCard('869934523596112', Validate::VOYAGER));
        $this->assertTrue(Validate::creditCard('869958670174621', Validate::VOYAGER));

        // Test multiple
        $this->assertTrue(Validate::creditCard('375239372816422', Vector {Validate::AMERICAN_EXPRESS, Validate::VISA})); // = amex
        $this->assertFalse(Validate::creditCard('869934523596112', Vector {Validate::AMERICAN_EXPRESS, Validate::VISA})); // = voyager

        // Test length
        $this->assertFalse(Validate::creditCard('2346533', Validate::MASTERCARD));
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidCreditCardException
     */
    public function testCreditCardInvalidType(): void {
        Validate::creditCard('6334768185398134', 'fakeCard');
    }

    public function testCurrency(): void {
        $this->assertTrue(Validate::currency('$1,000.00'));
        $this->assertTrue(Validate::currency('$343'));
        $this->assertTrue(Validate::currency('$193,482.33'));

        $this->assertFalse(Validate::currency('2,392.23'));
        $this->assertFalse(Validate::currency('2325'));
        $this->assertFalse(Validate::currency('$ten'));
        $this->assertFalse(Validate::currency('$1923.2'));
    }

    public function testCustom(): void {
        $this->assertTrue(Validate::custom('abcdef', '/^abc/'));
        $this->assertFalse(Validate::custom('abcdef', '/abc$/'));
    }

    public function testDate(): void {
        $this->assertTrue(Validate::date('2012-05-25'));
        $this->assertTrue(Validate::date('1946-09-11 12:03:43'));
        $this->assertTrue(Validate::date('March 25th 1993'));

        $this->assertFalse(Validate::date('02-32-2011'));
        $this->assertFalse(Validate::date('May 40th 2054'));
    }

    public function testDecimal(): void {
        $this->assertTrue(Validate::decimal('2923.23'));
        $this->assertTrue(Validate::decimal('1454.04'));
        $this->assertTrue(Validate::decimal('849383.938', 3));
        $this->assertTrue(Validate::decimal('+1234.54321', 5));
        $this->assertTrue(Validate::decimal('+0123.45e6', 0));
        $this->assertTrue(Validate::decimal('-0123.45e6', 0));
        $this->assertTrue(Validate::decimal('0123.45e6', 0));
        $this->assertTrue(Validate::decimal('1234.56'));
        $this->assertTrue(Validate::decimal('1234.01'));

        $this->assertFalse(Validate::decimal('2923'));
        $this->assertFalse(Validate::decimal('1454.0'));
        $this->assertFalse(Validate::decimal('849383.74235', 3));
    }

    public function testDimensions(): void {
        $this->assertTrue(Validate::dimensions($this->image, 'width', 200));
        $this->assertTrue(Validate::dimensions($this->image, 'height', 267));
        $this->assertFalse(Validate::dimensions($this->image, 'foobar', 267));

        $this->assertTrue(Validate::dimensions(['tmp_name' => $this->image, 'error' => 0], 'width', 200));
        $this->assertFalse(Validate::dimensions(['tmp_name' => 'fake.jpg', 'error' => 0], 'height', 267));
    }

    public function testEmail(): void {
        $this->assertTrue(Validate::email('email@titon.com', false));
        $this->assertTrue(Validate::email('email@sub.titon.com', false));
        $this->assertTrue(Validate::email('email+group@titon.com', false));
        $this->assertTrue(Validate::email('email+group@titon.co.uk', false));
        $this->assertTrue(Validate::email('email.dot@go-titon.com', false));
        $this->assertTrue(Validate::email('example@gmail.com', true));

        $this->assertFalse(Validate::email('email@titon', false));
        $this->assertFalse(Validate::email('email@titon.com.', false));
        $this->assertFalse(Validate::email('email@sub/titon.com', false));
        $this->assertFalse(Validate::email('email@sub:titon.com', false));
        $this->assertFalse(Validate::email('email@sub_titon.com', false));
        $this->assertFalse(Validate::email('email@somereallyfakedomain.com', true));
    }

    public function testEqual(): void {
        $this->assertTrue(Validate::equal('1', 1));
        $this->assertTrue(Validate::equal('abc', 'abc'));
        $this->assertTrue(Validate::equal(true, 1));

        $this->assertFalse(Validate::equal('1', 9));
        $this->assertFalse(Validate::equal('abc', 'foo'));
        $this->assertFalse(Validate::equal(true, false));
    }

    public function testExact(): void {
        $this->assertTrue(Validate::exact(1, 1));
        $this->assertTrue(Validate::exact('abc', 'abc'));
        $this->assertTrue(Validate::exact(true, true));

        $this->assertFalse(Validate::exact('1', 1));
        $this->assertFalse(Validate::exact(true, 1));
    }

    public function testExt(): void {
        $this->assertTrue(Validate::ext('image.gif'));
        $this->assertTrue(Validate::ext('image.jpeg'));
        $this->assertTrue(Validate::ext('doc.pdf', Vector {'pdf'}));
        $this->assertTrue(Validate::ext('web.HTML', Vector {'html', 'xhtml'}));

        $this->assertFalse(Validate::ext('image.bmp'));
        $this->assertFalse(Validate::ext('doc.doc', Vector {'pdf'}));
        $this->assertFalse(Validate::ext('web.XML', Vector {'html', 'xhtml'}));
    }

    public function testExtFile(): void {
        $this->assertTrue(Validate::ext(['name' => 'image.gif', 'error' => 0, 'type' => 'image/gif']));
    }

    public function testFile(): void {
        $this->assertTrue(Validate::file([
            'name' => 'file1.jpg',
            'type' => 'image/jpeg',
            'tmp_name' => '/tmp/phpUkYTB5',
            'error' => 0,
            'size' => 307808
        ]));

        $this->assertFalse(Validate::file('file1.jpg'));
        $this->assertFalse(Validate::file([
            'name' => 'file1.jpg',
            'type' => 'image/jpeg',
            'tmp_name' => '/tmp/phpUkYTB5',
            'error' => UPLOAD_ERR_CANT_WRITE,
            'size' => 307808
        ]));
    }

    public function testHeight(): void {
        $this->assertTrue(Validate::height($this->image, 267));
        $this->assertFalse(Validate::height($this->image, 233));
    }

    public function testInList(): void {
        $this->assertTrue(Validate::inList(1, Vector {1, '1', 'c'}));
        $this->assertTrue(Validate::inList('foo', Vector {'foo', 'BAR', 'wtf'}));

        $this->assertFalse(Validate::inList('b', Vector {1, '1', 'c'}));
        $this->assertFalse(Validate::inList('test', Vector {'foo', 'BAR', 'wtf'}));
    }

    public function testInRange(): void {
        $this->assertTrue(Validate::inRange(20, 10, 30));

        $this->assertFalse(Validate::inRange(35, 10, 30));
        $this->assertFalse(Validate::inRange(5, 10, 30));
    }

    public function testIp(): void {
        // both v4 and v6
        $this->assertTrue(Validate::ip('0.0.0.0'));
        $this->assertTrue(Validate::ip('192.168.1.156'));
        $this->assertTrue(Validate::ip('255.255.255.255'));
        $this->assertTrue(Validate::ip('2001:0db8:85a3:0000:0000:8a2e:0370:7334'));
        $this->assertFalse(Validate::ip('127.0.0'));
        $this->assertFalse(Validate::ip('127.0.0.a'));
        $this->assertFalse(Validate::ip('127.0.0.256'));
        $this->assertFalse(Validate::ip('2001:db8:85a3::8a2e:37023:7334'));

        // v4
        $this->assertTrue(Validate::ip('0.0.0.0', Validate::IPV4));
        $this->assertTrue(Validate::ip('192.168.1.156', Validate::IPV4));
        $this->assertTrue(Validate::ip('255.255.255.255', Validate::IPV4));
        $this->assertFalse(Validate::ip('127.0.0', Validate::IPV4));
        $this->assertFalse(Validate::ip('127.0.0.a', Validate::IPV4));
        $this->assertFalse(Validate::ip('127.0.0.256', Validate::IPV4));
        $this->assertFalse(Validate::ip('2001:0db8:85a3:0000:0000:8a2e:0370:7334', Validate::IPV4));

        // v6
        $this->assertTrue(Validate::ip('2001:0db8:85a3:0000:0000:8a2e:0370:7334', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:db8:85a3:0:0:8a2e:370:7334', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:db8:85a3::8a2e:370:7334', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:0000:0000:0000:0000:1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:0000:0000:0000::1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:0:0:0:0:1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:0:0::1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8::1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:db8::1428:57ab', Validate::IPV6));
        $this->assertTrue(Validate::ip('0000:0000:0000:0000:0000:0000:0000:0001', Validate::IPV6));
        $this->assertTrue(Validate::ip('::1', Validate::IPV6));
        $this->assertTrue(Validate::ip('::ffff:12.34.56.78', Validate::IPV6));
        $this->assertTrue(Validate::ip('::ffff:0c22:384e', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:1234:0000:0000:0000:0000:0000', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:0db8:1234:ffff:ffff:ffff:ffff:ffff', Validate::IPV6));
        $this->assertTrue(Validate::ip('2001:db8:a::123', Validate::IPV6));
        $this->assertTrue(Validate::ip('fe80::', Validate::IPV6));
        $this->assertTrue(Validate::ip('::ffff:192.0.2.128', Validate::IPV6));
        $this->assertTrue(Validate::ip('::ffff:c000:280', Validate::IPV6));
        $this->assertFalse(Validate::ip('123', Validate::IPV6));
        $this->assertFalse(Validate::ip('ldkfj', Validate::IPV6));
        $this->assertFalse(Validate::ip('2001::FFD3::57ab', Validate::IPV6));
        $this->assertFalse(Validate::ip('2001:db8:85a3::8a2e:37023:7334', Validate::IPV6));
        $this->assertFalse(Validate::ip('2001:db8:85a3::8a2e:370k:7334', Validate::IPV6));
        $this->assertFalse(Validate::ip('1:2:3:4:5:6:7:8:9', Validate::IPV6));
        $this->assertFalse(Validate::ip('1::2::3', Validate::IPV6));
        $this->assertFalse(Validate::ip('1:::3:4:5', Validate::IPV6));
        $this->assertFalse(Validate::ip('1:2:3::4:5:6:7:8:9', Validate::IPV6));
        $this->assertFalse(Validate::ip('::ffff:2.3.4', Validate::IPV6));
        $this->assertFalse(Validate::ip('::ffff:257.1.2.3', Validate::IPV6));
        $this->assertFalse(Validate::ip('255.255.255.255', Validate::IPV6));
    }

    public function testLuhn(): void {
        $this->assertTrue(Validate::luhn('370482756063980')); // American Express
        $this->assertTrue(Validate::luhn('5610745867413420')); // BankCard
        $this->assertTrue(Validate::luhn('30155483651028')); // Diners Club 14
        $this->assertTrue(Validate::luhn('36747701998969')); // 2004 MasterCard/Diners Club Alliance International 14
        $this->assertTrue(Validate::luhn('5597511346169950')); // 2004 MasterCard/Diners Club Alliance US & Canada 16
        $this->assertTrue(Validate::luhn('6011802876467237')); // Discover
        $this->assertTrue(Validate::luhn('201496944158937')); // enRoute
        $this->assertTrue(Validate::luhn('210034762247893')); // JCB 15 digit
        $this->assertTrue(Validate::luhn('3096806857839939')); // JCB 16 digit
        $this->assertTrue(Validate::luhn('5020147409985219')); // Maestro (debit card)
        $this->assertTrue(Validate::luhn('5580424361774366')); // Mastercard
        $this->assertTrue(Validate::luhn('6767432107064987')); // Solo 16
        $this->assertTrue(Validate::luhn('676714834398858593')); // Solo 18
        $this->assertTrue(Validate::luhn('6767838565218340113')); // Solo 19
        $this->assertTrue(Validate::luhn('5641829171515733')); // Switch 16
        $this->assertTrue(Validate::luhn('493622764224625174')); // Switch 18
        $this->assertTrue(Validate::luhn('6759603460617628716')); // Switch 19
        $this->assertTrue(Validate::luhn('4024007174754')); // VISA 13 digit
        $this->assertTrue(Validate::luhn('4916375389940009')); // VISA 16 digit
        $this->assertTrue(Validate::luhn('4175003346287100')); // Visa Electron
        $this->assertTrue(Validate::luhn('869940697287073')); // Voyager

        $this->assertFalse(Validate::luhn('466752418696342'));
        $this->assertFalse(Validate::luhn('869940697287173'));
    }

    public function testMimeType(): void {
        $this->assertTrue(Validate::mimeType($this->image, Vector {'image/jpeg', 'image/jpg'}));
        $this->assertFalse(Validate::mimeType($this->image, Vector {'image/gif'}));

        $this->assertTrue(Validate::mimeType(['tmp_name' => $this->image, 'error' => 0], Vector {'image/jpeg', 'image/jpg'}));
        $this->assertFalse(Validate::mimeType(['tmp_name' => 'fake.jpg', 'error' => 0], Vector {'image/gif'}));
    }

    public function testMinFilesize(): void {
        $this->assertTrue(Validate::minFilesize($this->image, 13437));
        $this->assertTrue(Validate::minFilesize(['tmp_name' => $this->image, 'error' => 0, 'size' => 13437], 10000));
        $this->assertFalse(Validate::minFilesize($this->image, 15000));
        $this->assertFalse(Validate::minFilesize(['tmp_name' => $this->image, 'error' => 0, 'size' => 13437], 13458));
        $this->assertFalse(Validate::minFilesize('fake.jpg', 13458));
    }

    public function testMinHeight(): void {
        $this->assertTrue(Validate::minHeight($this->image, 267));
        $this->assertTrue(Validate::minHeight($this->image, 255));
        $this->assertFalse(Validate::minHeight($this->image, 300));
        $this->assertFalse(Validate::minHeight($this->image, 268));
    }

    public function testMinLength(): void {
        $this->assertTrue(Validate::minLength('This string is enough', 20));
        $this->assertFalse(Validate::minLength('This is too short', 20));
    }

    public function testMinWidth(): void {
        $this->assertTrue(Validate::minWidth($this->image, 200));
        $this->assertTrue(Validate::minWidth($this->image, 155));
        $this->assertFalse(Validate::minWidth($this->image, 215));
        $this->assertFalse(Validate::minWidth($this->image, 355));
    }

    public function testMaxFilesize(): void {
        $this->assertTrue(Validate::maxFilesize($this->image, 13437));
        $this->assertTrue(Validate::maxFilesize(['tmp_name' => $this->image, 'error' => 0, 'size' => 13437], 15000));
        $this->assertFalse(Validate::maxFilesize($this->image, 13000));
        $this->assertFalse(Validate::maxFilesize(['tmp_name' => $this->image, 'error' => 0, 'size' => 13437], 12233));
        $this->assertFalse(Validate::maxFilesize('fakq.jpg', 13000));
    }

    public function testMaxHeight(): void {
        $this->assertTrue(Validate::maxHeight($this->image, 267));
        $this->assertTrue(Validate::maxHeight($this->image, 300));
        $this->assertFalse(Validate::maxHeight($this->image, 265));
        $this->assertFalse(Validate::maxHeight($this->image, 144));
    }

    public function testMaxLength(): void {
        $this->assertTrue(Validate::maxLength('This is just right', 20));
        $this->assertFalse(Validate::maxLength('This is too far too long', 20));
    }

    public function testMaxWidth(): void {
        $this->assertTrue(Validate::maxWidth($this->image, 200));
        $this->assertTrue(Validate::maxWidth($this->image, 255));
        $this->assertFalse(Validate::maxWidth($this->image, 100));
        $this->assertFalse(Validate::maxWidth($this->image, 199));
    }

    public function testNotEmpty(): void {
        $this->assertTrue(Validate::notEmpty('abc'));
        $this->assertTrue(Validate::notEmpty(123));
        $this->assertTrue(Validate::notEmpty(['foo', 'bar']));
        $this->assertTrue(Validate::notEmpty(true));
        $this->assertTrue(Validate::notEmpty(0));
        $this->assertTrue(Validate::notEmpty('0'));

        $this->assertFalse(Validate::notEmpty(''));
        $this->assertFalse(Validate::notEmpty(false));
        $this->assertFalse(Validate::notEmpty(null));
    }

    public function testNumeric(): void {
        $this->assertTrue(Validate::numeric('1234'));
        $this->assertTrue(Validate::numeric(456));
        $this->assertFalse(Validate::numeric('abc34f'));
    }

    public function testPhone(): void {
        $this->assertTrue(Validate::phone('666-1337'));
        $this->assertTrue(Validate::phone('(888)666-1337'));
        $this->assertTrue(Validate::phone('(888) 666-1337'));
        $this->assertTrue(Validate::phone('1 (888) 666-1337'));
        $this->assertTrue(Validate::phone('+1 (888) 666-1337'));

        $this->assertFalse(Validate::phone('666.1337'));
        $this->assertFalse(Validate::phone('888-666.1337'));
        $this->assertFalse(Validate::phone('1 888.666.1337'));
        $this->assertFalse(Validate::phone('666-ABMS'));
    }

    public function testPostalCode(): void {
        $this->assertTrue(Validate::postalCode('38842'));
        $this->assertTrue(Validate::postalCode('38842-0384'));

        $this->assertFalse(Validate::postalCode('3842'));
        $this->assertFalse(Validate::postalCode('38842.0384'));
        $this->assertFalse(Validate::postalCode('AksiS-0384'));
    }

    public function testSsn(): void {
        $this->assertTrue(Validate::ssn('666-10-1337'));
        $this->assertTrue(Validate::ssn('384-29-3481'));

        $this->assertFalse(Validate::ssn('66-10-1337'));
        $this->assertFalse(Validate::ssn('384-29-341'));
        $this->assertFalse(Validate::ssn('666.10.1337'));
        $this->assertFalse(Validate::ssn('AHE-29-34P1'));
    }

    public function testUuid(): void {
        $this->assertTrue(Validate::uuid('a8293fde-ce92-9abe-83de-7294ab29cd03'));

        $this->assertFalse(Validate::uuid('a8293fde-ce92-83de-7294ab29cd03'));
        $this->assertFalse(Validate::uuid('a8293de-ce92-9abe-83de-7294ab29cd'));
        $this->assertFalse(Validate::uuid('a8293kq-ce92-9abe-83de-729pdu29cd03'));
    }

    public function testUrl(): void {
        $this->assertTrue(Validate::url('http://titon'));
        $this->assertTrue(Validate::url('http://titon.com'));
        $this->assertTrue(Validate::url('http://titon.com?query=string'));
        $this->assertTrue(Validate::url('http://titon.com?query=string&key=value'));
        $this->assertTrue(Validate::url('http://titon.com?query=string&key=value#fragment'));
        $this->assertTrue(Validate::url('http://titon.com:8080?query=string&key=value#fragment'));
        $this->assertTrue(Validate::url('http://sub.titon.com:8080?query=string&key=value#fragment'));
        $this->assertTrue(Validate::url('https://sub.titon.com:8080?query=string&key=value#fragment'));
        $this->assertTrue(Validate::url('http://titon.com/some/path'));
        $this->assertTrue(Validate::url('http://go-titon.com'));
        $this->assertTrue(Validate::url('http://127.29.12.34/some/path'));
        $this->assertTrue(Validate::url('ftp://user:password@titon.com:22'));
        $this->assertTrue(Validate::url('ftp://127.29.12.34'));
        $this->assertTrue(Validate::url('ftp://127.29.12.34/some/path'));

        $this->assertFalse(Validate::url('http://go_titon.com'));
        $this->assertFalse(Validate::url('titon.com'));
        $this->assertFalse(Validate::url('www.titon.com'));
    }

    public function testWidth(): void {
        $this->assertTrue(Validate::width($this->image, 200));
        $this->assertFalse(Validate::width($this->image, 100));
    }

}
