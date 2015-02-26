<?hh
namespace Titon\Utility;

use Titon\Test\TestCase;

class FormatTest extends TestCase {

    public function testAtom(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('1988-02-26T16:35:00+00:00', Format::atom($time));
    }

    public function testDate(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('1988-02-26', Format::date($time));
        $this->assertEquals('02/26/1988', Format::date($time, '%m/%d/%Y'));
    }

    public function testDatetime(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('1988-02-26 16:35:00', Format::datetime($time));
        $this->assertEquals('02/26/1988 04:35PM', Format::datetime($time, '%m/%d/%Y %I:%M%p'));
    }

    public function testFormat(): void {
        $this->assertEquals('(123) 456', Format::format('1234567890', '(###) ###'));
        $this->assertEquals('(123) 456-7890', Format::format('1234567890', '(###) ###-####'));
        $this->assertEquals('(123) 456-####', Format::format('123456', '(###) ###-####'));

        $this->assertEquals('123.456', Format::format('1234567890', '###.###'));
        $this->assertEquals('123.456.7890', Format::format('1234567890', '###.###.####'));
        $this->assertEquals('123.456.####', Format::format('123456', '###.###.####'));

        // credit card
        $this->assertEquals('3772-3483-0461-4543', Format::format('3772348304614543', '####-####-####-####'));

        // credit card with mask
        $this->assertEquals('****-****-****-4543', Format::format('3772348304614543', '****-****-****-####'));

        // longer number
        $this->assertEquals('3772-3483-0461-4543', Format::format('377234830461454313', '####-####-####-####'));
    }

    public function testHttp(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('Fri, 26 Feb 1988 16:35:00 GMT', Format::http($time));
    }

    public function testPhone(): void {
        $formats = Map {
            7 => '###-####',
            10 => '(###) ###-####',
            11 => '# (###) ###-####'
        };

        $this->assertEquals('666-1337', Format::phone('6661337', $formats));
        $this->assertEquals('(888) 666-1337', Format::phone('8886661337', $formats));
        $this->assertEquals('1 (888) 666-1337', Format::phone('+1 8886661337', $formats));
    }

    public function testRelativeTime(): void {
        $time = mktime(16, 35, 0, 2, 26, 2012);

        // Current
        $this->assertEquals('just now', Format::relativeTime(strtotime('+0 seconds', $time), Map {'time' => $time}));
        $this->assertEquals('just now', Format::relativeTime($time, Map {'time' => $time}));
        $this->assertEquals('in 2 hours', Format::relativeTime(strtotime('+2 hours', $time), Map {'time' => $time}));
        $this->assertEquals('in 45 minutes', Format::relativeTime(strtotime('+45 minutes', $time), Map {'time' => $time}));

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
        $this->assertEquals('in 45 MINS', Format::relativeTime(strtotime('+45 minutes', $time), Map {'time' => $time}, Map {
            'minutes' => Map {2 => '{count} MINS'}
        }));
    }

    public function testRss(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('Fri, 26 Feb 1988 16:35:00 +0000', Format::rss($time));
    }

    public function testSsn(): void {
        $this->assertEquals('998-29-3841', Format::ssn('998293841', '###-##-####'));
    }

    public function testTime(): void {
        $time = mktime(16, 35, 0, 2, 26, 1988);

        $this->assertEquals('16:35:00', Format::time($time));
        $this->assertEquals('04:35PM', Format::time($time, '%I:%M%p'));
    }

}
