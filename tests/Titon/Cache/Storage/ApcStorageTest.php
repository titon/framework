<?php
namespace Titon\Cache\Storage;

class ApcStorageTest extends AbstractStorageTest {

    protected function setUp() {
        if (!extension_loaded('apc')) {
            $this->markTestSkipped('APC is not installed or configured properly');
        }

        $this->object = new ApcStorage();

        parent::setUp();
    }

}