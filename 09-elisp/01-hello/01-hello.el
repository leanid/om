;;; package --- Summary
;;; Commentary:
;;; hello To execute next line in doomemacs
;; set pointer on it and press: SPC+m+e+d
;;; Code:
(defun hello () "hello world!")
;; how to create constant in elisp? example?
(defconst MY-CONSTANT 10 "This is my constant")
;; how to create variable in elisp? example?
(defvar my-variable 10 "This is my variable")
;; to define a user option
(defcustom my-option 10 "This is my option" :type 'integer :group 'my-group)
;; to define a buffer local variable, use
(defvar-local my-buffer-local-variable 10 "This is my buffer local variable")
;; elisp types:
;; integer,       1
;; float,         1.0
;; string,        "hello"
;; symbol,        'hello      (') - means do not evaluate
;; keyword,       :hello
;; chars,         ?a
;; boolean,       t, nil
;; pair,          (1 . 2) or (1 2) or (cons 1 2)
;; list,          (list 1 2 3)
;; vector,        [1 2 3]
;; hash-table,    (make-hash-table)
;; buffer,        (get-buffer "foo")
;; window,        (selected-window)
;; frame,         (selected-frame)
;; process,       (get-process "emacs")
;; marker,        (point-marker)
;; overlay,       (make-overlay 1 2)
;; and many more
`(1 2 ,(+ 1 2)) ;; => (1 2 3)    (,) - means evaluate
'(1 2 (+ 1 2)) ;; => (1 2 (+ 1 2))
`(1 2 (+ 1 2)) ;; => (1 2 (+ 1 2))
;; to execute call to newlly created function
;; set pointer to last ')' and press: SPC+m+e+e
(hello)
(defun insert-numbers ()
  "Insert numbers from 1 to 5."
  (interactive)
  (dotimes (i 5)
    (insert (number-to-string (+ 1 i)))))

(message "Hello, world!") ;; => "Hello, world!"
(global-set-key (kbd "C-S-s") 'insert-numbers) ;; press Ctrl+Shift+s to execute insert-numbers
(keymap-global-set "C-S-s" 'insert-numbers) ;; same as above
(keymap-set cpp-mode-map "C-S-s" 'insert-numbers) ;; press Ctrl+Shift+s in cpp-mode to execute insert-numbers
(if t
    'it-was-true
  'it-was-false) ;; => it-was-true
(when t
  'it-was-true) ;; => it-was-true
(unless nil
  'it-was-true) ;; => it-was-true
(and t t) ;; => t
(or nil t) ;; => t
(not nil) ;; => t
(if t (progn (message "It was true")
             (message "next message")
             (message "last message"))
  (message "It was false")) ;; => "It was true"
(let ((x 42))
  (cond ((= x 0) 'zero)
        ((> x 0) 'positive)
        (t 'negative)))
;;; setting variables
(setq my-variable 42)
(setopt my-option 42) ;; has type checking from defcustom
;;; 01-hello.el ends here
