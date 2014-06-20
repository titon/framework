<?php
namespace Titon\Utility;

use Titon\Test\TestCase;
use \DateTime;

class TimeTest extends TestCase {

    public function testDifference() {
        $this->assertEquals(-60, Time::difference(null, '+60 seconds'));
        $this->assertEquals(-3600, Time::difference(new DateTime(), '+1 hour'));
        $this->assertEquals(14400, Time::difference('+5 hours', '+1 hour'));
    }

    public function testFactory() {
        $this->assertInstanceOf('\DateTime', Time::factory());
        $this->assertInstanceOf('\DateTime', Time::factory('1988-02-26 12:23:12'));
        $this->assertInstanceOf('\DateTime', Time::factory(time()));

        $dt = new DateTime();
        $this->assertSame($dt, Time::factory($dt));
    }

    public function testIsToday() {
        $this->assertTrue(Time::isToday('+0 day'));
        $this->assertFalse(Time::isToday('+2 day'));
        $this->assertFalse(Time::isToday('-2 day'));
    }

    public function testIsThisWeek() {
        $this->assertTrue(Time::isThisWeek('+0 week'));
        $this->assertFalse(Time::isThisWeek('+2 week'));
        $this->assertFalse(Time::isThisWeek('-2 week'));
    }

    public function testIsThisMonth() {
        $this->assertTrue(Time::isThisMonth('+0 month'));
        $this->assertFalse(Time::isThisMonth('+2 month'));
        $this->assertFalse(Time::isThisMonth('-2 month'));
    }

    public function testIsThisYear() {
        $this->assertTrue(Time::isThisYear('+0 year'));
        $this->assertFalse(Time::isThisYear('+2 year'));
        $this->assertFalse(Time::isThisYear('-2 year'));
    }

    public function testIsTomorrow() {
        $this->assertTrue(Time::isTomorrow('+1 day'));
        $this->assertFalse(Time::isTomorrow('+0 day'));
        $this->assertFalse(Time::isTomorrow('-1 day'));
    }

    public function testIsWithinNext() {
        $this->assertTrue(Time::isWithinNext('+1 day', '+7 days'));
        $this->assertTrue(Time::isWithinNext('+37 hours', '+7 days'));
        $this->assertTrue(Time::isWithinNext('+1 week', '+7 days'));

        $this->assertFalse(Time::isWithinNext('-1 day', '+7 days'));
        $this->assertFalse(Time::isWithinNext('+8 days', '+7 days'));
    }

    public function testTimezone() {
        $this->assertInstanceOf('\DateTimeZone', Time::timezone());

        $dtz = new \DateTimeZone('America/Los_Angeles');
        $this->assertSame($dtz, Time::timezone($dtz));
    }

    public function testToUnix() {
        $this->assertTrue(is_numeric(Time::toUnix(null)));
        $this->assertTrue(is_numeric(Time::toUnix(time())));
        $this->assertTrue(is_numeric(Time::toUnix('+1 week')));
        $this->assertTrue(is_numeric(Time::toUnix(new DateTime())));

        $this->assertFalse(is_numeric(Time::toUnix('string')));
    }

    public function testWasLastWeek() {
        $this->assertTrue(Time::wasLastWeek('last week'));
        $this->assertFalse(Time::wasLastWeek('next week'));
    }

    public function testWasLastMonth() {
        $this->assertTrue(Time::wasLastMonth('-31 days'));
        $this->assertFalse(Time::wasLastMonth('+31 days'));
        $this->assertFalse(Time::wasLastMonth('+0 month'));
    }

    public function testWasLastYear() {
        $this->assertTrue(Time::wasLastYear('-1 year'));
        $this->assertFalse(Time::wasLastYear('+1 year'));
        $this->assertFalse(Time::wasLastYear('+0 year'));
    }

    public function testWasYesterday() {
        $this->assertTrue(Time::wasYesterday('-1 day'));
        $this->assertFalse(Time::wasYesterday('+0 day'));
        $this->assertFalse(Time::wasYesterday('+1 day'));
    }

    public function testWasWithinLast() {
        $this->assertTrue(Time::wasWithinLast('-1 day', '-7 days'));
        $this->assertTrue(Time::wasWithinLast('-37 hours', '-7 days'));
        $this->assertTrue(Time::wasWithinLast('-1 week', '-7 days'));

        $this->assertFalse(Time::wasWithinLast('+1 day', '-7 days'));
        $this->assertFalse(Time::wasWithinLast('-8 days', '-7 days'));
    }

}