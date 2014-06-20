<?php
namespace Titon\Utility;

use Titon\Test\TestCase;

/**
 * @property \Titon\Utility\Validator $object
 */
class ValidatorTest extends TestCase {

    protected function setUp() {
        parent::setUp();

        $this->object = new Validator(array(
            'username' => 'miles',
            'email' => 'miles@titon' // invalid
        ));
    }

    public function testAddGetErrors() {
        $this->assertEquals(array(), $this->object->getErrors());

        $this->object->addError('username', 'Invalid username');
        $this->assertEquals(array('username' => 'Invalid username'), $this->object->getErrors());
    }

    public function testAddGetFields() {
        $this->assertEquals(array(), $this->object->getFields());

        $this->object->addField('username', 'Username');
        $this->assertEquals(array('username' => 'Username'), $this->object->getFields());
    }

    public function testAddGetRules() {
        $this->assertEquals(array(), $this->object->getRules());

        // via addRule()
        $this->object
            ->addField('basicRule', 'Basic Rule')
                ->addRule('basicRule', 'alphaNumeric', 'Custom alpha-numeric message')
                ->addRule('basicRule', 'between', 'May only be between 0 and 100 characters', array(0, 100)); // use default message

        $this->assertEquals(array(
            'basicRule' => array(
                'alphaNumeric' => array(
                    'message' => 'Custom alpha-numeric message',
                    'options' => array()
                ),
                'between' => array(
                    'message' => 'May only be between 0 and 100 characters',
                    'options' => array(0, 100)
                )
            )
        ), $this->object->getRules());

        // via third argument in addField()
        $this->object->addMessages(array(
            'phone' => 'Invalid phone number',
            'email' => 'Please provide an email',
            'ext' => 'Valid extensions are {0}',
            'ip' => 'Please provide an IPv4'
        ));

        $this->object->addField('advRule', 'Advanced Rule', array(
            'phone',
            'email',
            'ext' => array(array('txt', 'pdf')),
            'ip' => Validate::IPV4
        ));

        $this->assertEquals(array(
            'basicRule' => array(
                'alphaNumeric' => array(
                    'message' => 'Custom alpha-numeric message',
                    'options' => array()
                ),
                'between' => array(
                    'message' => 'May only be between 0 and 100 characters',
                    'options' => array(0, 100)
                )
            ),
            'advRule' => array(
                'phone' => array(
                    'message' => 'Invalid phone number',
                    'options' => array()
                ),
                'email' => array(
                    'message' => 'Please provide an email',
                    'options' => array()
                ),
                'ext' => array(
                    'message' => 'Valid extensions are {0}',
                    'options' => array(array('txt', 'pdf'))
                ),
                'ip' => array(
                    'message' => 'Please provide an IPv4',
                    'options' => array(Validate::IPV4)
                )
            )
        ), $this->object->getRules());
    }

    public function testMessages() {
        $this->object
            ->addField('username', 'Username')
                ->addRule('username', 'between', 'May only be between {0} and {1} characters', array(25, 45))
            ->addField('email', 'Email')
                ->addRule('email', 'ext', 'May only have extension: {0}', array(array('gif', 'png')));

        $this->assertEquals(array(
            'between' => 'May only be between {0} and {1} characters',
            'ext' => 'May only have extension: {0}'
        ), $this->object->getMessages());

        $this->object->validate();

        $this->assertEquals(array(
            'username' => 'May only be between 25 and 45 characters',
            'email' => 'May only have extension: gif, png'
        ), $this->object->getErrors());
    }

    public function testMessageDetection() {
        $this->object
            ->addField('username', 'Username')
                ->addRule('username', 'notEmpty', null)
                ->addRule('username', 'between', '{field} may only be between {0} and {1} characters', array(10, 20))
                ->addRule('username', 'alpha', null)
            ->addMessages(array(
                'notEmpty' => '{field} cannot be empty'
            ));

        $this->object->addMessages(array(
            'alpha' => '{title} may only be alpha'
        ));

        // {field} insertion
        $this->object->validate();

        $this->assertEquals(array(
            'username' => 'username may only be between 10 and 20 characters'
        ), $this->object->getErrors());

        // {title} insertion
        $this->object->setData(array('username' => 'foo long name bar!'));
        $this->object->validate();

        $this->assertEquals(array(
            'username' => 'Username may only be alpha'
        ), $this->object->getErrors());
    }

    /**
     * @expectedException \Titon\Utility\Exception\InvalidValidationRuleException
     */
    public function testMessagesErrorOnMissing() {
        $this->object
            ->addField('username', 'Username')
                ->addRule('username', 'notEmpty', null);

        $this->object->validate();
    }

    public function testValidate() {
        $this->object->addField('username', 'Username')->addRule('username', 'alpha', 'Not alpha');

        $this->assertTrue($this->object->validate());
        $this->assertEquals(array(), $this->object->getErrors());

        // this will fail
        $this->object->addField('email', 'Email')->addRule('email', 'email', 'Invalid email');

        $this->assertFalse($this->object->validate());
        $this->assertEquals(array('email' => 'Invalid email'), $this->object->getErrors());
    }

    public function testSplitShorthand() {
        // only a rule
        $this->assertEquals(array(
            'rule' => 'boolean',
            'message' => '',
            'options' => array()
        ), $this->object->splitShorthand('boolean'));

        // rule with 1 param
        $this->assertEquals(array(
            'rule' => 'decimal',
            'message' => '',
            'options' => array(1)
        ), $this->object->splitShorthand('decimal:1'));

        // rule with 2 params
        $this->assertEquals(array(
            'rule' => 'between',
            'message' => '',
            'options' => array(1, 10)
        ), $this->object->splitShorthand('between:1,10'));

        // only a rule and message
        $this->assertEquals(array(
            'rule' => 'boolean',
            'message' => 'Must be a boolean!',
            'options' => array()
        ), $this->object->splitShorthand('boolean::Must be a boolean!'));

        // rule with 2 params and message
        $this->assertEquals(array(
            'rule' => 'between',
            'message' => 'Must be between 1:10!',
            'options' => array(1, 10)
        ), $this->object->splitShorthand('between:1,10:Must be between 1:10!'));
    }

    public function testMakeFromShorthand() {
        // simple rule
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => 'alphaNumeric'
        ));

        $this->assertEquals(array(
            'field' => array(
                'alphaNumeric' => array(
                    'message' => '',
                    'options' => array()
                )
            )
        ), $obj->getRules());

        // simple 2 rules
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => 'alphaNumeric|boolean'
        ));

        $this->assertEquals(array(
            'field' => array(
                'alphaNumeric' => array(
                    'message' => '',
                    'options' => array()
                ),
                'boolean' => array(
                    'message' => '',
                    'options' => array()
                )
            )
        ), $obj->getRules());

        // simple 2 rules with options
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => 'between:5,10|equal:7'
        ));

        $this->assertEquals(array(
            'field' => array(
                'between' => array(
                    'message' => '',
                    'options' => array(5, 10)
                ),
                'equal' => array(
                    'message' => '',
                    'options' => array(7)
                )
            )
        ), $obj->getRules());

        // split 2 rules
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => array('between:5,10', 'equal:7')
        ));

        $this->assertEquals(array(
            'field' => array(
                'between' => array(
                    'message' => '',
                    'options' => array(5, 10)
                ),
                'equal' => array(
                    'message' => '',
                    'options' => array(7)
                )
            )
        ), $obj->getRules());

        // nested 2 rules
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => array(
                'title' => 'Field',
                'rules' => 'between:5,10|equal:7'
            )
        ));

        $this->assertEquals(array(
            'field' => array(
                'between' => array(
                    'message' => '',
                    'options' => array(5, 10)
                ),
                'equal' => array(
                    'message' => '',
                    'options' => array(7)
                )
            )
        ), $obj->getRules());

        // nested split 2 rules
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => array(
                'title' => 'Field',
                'rules' => array('between:5,10', 'equal:7')
            )
        ));

        $this->assertEquals(array(
            'field' => array(
                'between' => array(
                    'message' => '',
                    'options' => array(5, 10)
                ),
                'equal' => array(
                    'message' => '',
                    'options' => array(7)
                )
            )
        ), $obj->getRules());

        // advanced multiple rules
        $obj = Validator::makeFromShorthand(array(), array(
            'field' => array(
                'rules' => array(
                    'phone::Invalid phone number',
                    'email::Please provide an email',
                    'ext:txt:Valid extensions are txt, pdf',
                    'ip:' . Validate::IPV4 . ':Please provide an IPv4'
                )
            )
        ));

        $this->assertEquals(array(
            'field' => array(
                'phone' => array(
                    'message' => 'Invalid phone number',
                    'options' => array()
                ),
                'email' => array(
                    'message' => 'Please provide an email',
                    'options' => array()
                ),
                'ext' => array(
                    'message' => 'Valid extensions are txt, pdf',
                    'options' => array('txt')
                ),
                'ip' => array(
                    'message' => 'Please provide an IPv4',
                    'options' => array(Validate::IPV4)
                )
            )
        ), $obj->getRules());
    }

}