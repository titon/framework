# Validate #

Provides data validation through an easy to use validator and constraint system. Validators manage fields, 
each with their own set of rules and error messages, as well as a mapping of constraints to validate against.

```hack
$validator = new Titon\Validate\CoreValidator();
$validator->addField('username', 'Username')
    ->addRule('username', 'notEmpty', '{title} is required')
    ->addRule('username', 'alphaNumeric', '{title} may only contain alpha-numeric characters')
    ->addRule('username', 'between', '{title} may only be between {0} and {1} characters', Vector {25, 45});
    
if ($validator->validate($data)) {
    // ...
}
```