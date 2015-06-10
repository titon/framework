<?hh
namespace Titon\Intl\Utility;

use Titon\Context\Depository;
use Titon\Intl\Locale;
use Titon\Intl\MessageLoader;
use Titon\Intl\Translator;
use Titon\Io\Reader\HackReader;
use Titon\Test\TestCase;

/**
 * @property \Titon\Intl\Translator $object
 */
class FormatTest extends TestCase {

    protected function setUp(): void {
        parent::setUp();

        $this->object = new Translator(new MessageLoader(Vector {new HackReader()}));
        $this->object->addResourcePaths('common', Set {SRC_DIR . '/Titon/Intl/'});
        $this->object->addLocale(new Locale('en_US'));
        $this->object->localize('en_US');

        Depository::getInstance()->register('Titon\Intl\Translator', $this->object);
    }

    protected function tearDown(): void {
        Depository::getInstance()->clear();
    }

    public function testDate(): void {
        $this->assertEquals('02/26/1988', Format::date(mktime(16, 35, 0, 2, 26, 1988)));
    }

    public function testDatetime(): void {
        $this->assertEquals('02/26/1988 04:35PM', Format::datetime(mktime(16, 35, 0, 2, 26, 1988)));
    }

    public function testPhone(): void {
        $this->assertEquals('666-1337', Format::phone('6661337'));
        $this->assertEquals('(888) 666-1337', Format::phone('8886661337'));
        $this->assertEquals('1 (888) 666-1337', Format::phone('+1 8886661337'));
        $this->assertEquals('6 6 6 - 1 3 3 7', Format::phone('6661337', Map {
            7 => '# # # - # # # #'
        }));
    }

    public function testRelativeTime(): void {
        $time = mktime(16, 35, 0, 2, 26, 2012);

        // Current
        $this->assertEquals('just now', Format::relativeTime(strtotime('+0 seconds', $time), Map {'time' => $time}));
        $this->assertEquals('just now', Format::relativeTime($time, Map {'time' => $time}));

        // Past
        $this->assertEquals('45 seconds ago', Format::relativeTime(strtotime('-45 seconds', $time), Map {'time' => $time}));
        $this->assertEquals('2 weeks, 2 days ago', Format::relativeTime(strtotime('-16 days', $time), Map {'time' => $time}));
        $this->assertEquals('8 months ago', Format::relativeTime(strtotime('-33 weeks', $time), Map {'time' => $time}));
        $this->assertEquals('6 months, 4 days ago', Format::relativeTime(strtotime('-6 months', $time), Map {'time' => $time}));
        $this->assertEquals('2 years, 2 months ago', Format::relativeTime(strtotime('-799 days', $time), Map {'time' => $time}));

        // Future
        $this->assertEquals('in 45 seconds', Format::relativeTime(strtotime('+45 seconds', $time), Map {'time' => $time}));
        $this->assertEquals('in 2 weeks, 2 days', Format::relativeTime(strtotime('+16 days', $time), Map {'time' => $time}));
        $this->assertEquals('in 8 months', Format::relativeTime(strtotime('+33 weeks', $time), Map {'time' => $time}));
        $this->assertEquals('in 6 months, 2 days', Format::relativeTime(strtotime('+6 months', $time), Map {'time' => $time}));
        $this->assertEquals('in 2 years, 2 months', Format::relativeTime(strtotime('+799 days', $time), Map {'time' => $time}));

        // Large depth
        $this->assertEquals('1 year, 1 month, 4 days ago', Format::relativeTime(strtotime('-399 days', $time), Map {'time' => $time, 'depth' => 5}));
        $this->assertEquals('1 year, 3 months ago', Format::relativeTime(strtotime('-444 days', $time), Map {'time' => $time, 'depth' => 5}));
        $this->assertEquals('3 years ago', Format::relativeTime(strtotime('-999 days', $time), Map {'time' => $time, 'depth' => 5}));
        $this->assertEquals('3 years, 5 months ago', Format::relativeTime(strtotime('-1235 days', $time), Map {'time' => $time, 'depth' => 5}));
        $this->assertEquals('2 years, 2 months, 1 week, 2 days ago', Format::relativeTime(strtotime('-799 days', $time), Map {'time' => $time, 'depth' => 5}));

        // Non-verbose
        $this->assertEquals('2y, 2m, 1w, 2d ago', Format::relativeTime(strtotime('-799 days', $time), Map {'time' => $time, 'depth' => 5, 'verbose' => false}));
        $this->assertEquals('in 2y, 2m, 1w, 2d', Format::relativeTime(strtotime('+799 days', $time), Map {'time' => $time, 'depth' => 5, 'verbose' => false}));

        // Custom messages
        $this->assertEquals('in 5 DAYS BRO', Format::relativeTime(strtotime('+5 days', $time), Map {'time' => $time}, Map {
            'days' => Map {2 => '{count} DAYS BRO'}
        }));
    }

    public function testSsn(): void {
        $this->assertEquals('998-29-3841', Format::ssn('998293841'));
    }

    public function testTime(): void {
        $this->assertEquals('04:35PM', Format::time(mktime(16, 35, 0, 2, 26, 1988)));
    }

}
