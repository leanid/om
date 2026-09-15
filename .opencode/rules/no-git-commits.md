# No Git Commits

- **NEVER** run `git commit`, `git add`, `git rm`, `git push`, or any other commands that modify the git state.
- The user will handle all git commits and state modifications manually.
- You are allowed to run read-only git commands like `git status`, `git diff`, `git log`, etc., to gather context.
- You are allowed to run other programs, searches, and build commands.
- Do not proactively stage or commit changes even if a task is completed.

Enforced mechanically by `opencode.json` → `permission.bash` deny rules
(`git add*`, `git commit*`, `git push*`, `git rm*`, `git reset*`, `git rebase*`).
